use cpu::CPUError;
use eframe;
use egui;
use rfd::FileDialog;
use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::sync::{
    atomic::{AtomicBool, Ordering},
    Arc, Mutex,
};
use std::thread;
mod cpu;

static mut HZ: f64 = 0.0;

struct GUI {
    cpu: Arc<Mutex<cpu::CPU>>,
    running: Arc<AtomicBool>,
    search_ram: String,
    error_pc_out_of_bounds: Arc<AtomicBool>,
    error_halt: Arc<AtomicBool>,
}

impl GUI {
    fn new(_cc: &eframe::CreationContext<'_>, cpu: cpu::CPU) -> Self {
        Self {
            cpu: Arc::new(Mutex::new(cpu)),
            running: Arc::new(AtomicBool::new(false)),
            search_ram: String::new(),
            error_pc_out_of_bounds: Arc::new(AtomicBool::new(false)),
            error_halt: Arc::new(AtomicBool::new(false)),
        }
    }

    fn start_execution(&self) {
        let cpu_ref = Arc::clone(&self.cpu);
        let running = Arc::clone(&self.running);
        let error_pc_out_of_bounds = Arc::clone(&self.error_pc_out_of_bounds);
        let error_halt = Arc::clone(&self.error_halt);
        running.store(true, Ordering::SeqCst);
        thread::spawn(move || {
            while running.load(Ordering::SeqCst) {
                let time: f32;
                let l_clock: u64;
                if let Ok(mut cpu) = cpu_ref.lock() {
                    l_clock = cpu.clock;
                    // Assuming the cpu struct has a field `last_update_time` of type `std::time::Instant`
                    let x = 1e9 / (std::time::Instant::now() - cpu.l_executed_t).as_nanos() as f64;
                    let error = cpu.execute_instruction(false, 0);
                    match error {
                        CPUError::Ok => {}
                        CPUError::PcOutOfBounds => {
                            running.store(false, Ordering::SeqCst);
                            error_pc_out_of_bounds.store(true, Ordering::SeqCst);
                        }
                        CPUError::Halt => {
                            // running.store(false, Ordering::SeqCst);
                            // error_halt.store(true, Ordering::SeqCst);
                        }
                    }
                    let current_hz = x * (cpu.clock - l_clock) as f64;
                    let now = std::time::Instant::now();
                    if now.duration_since(cpu.last_update_time).as_secs() >= 1 {
                        cpu.last_update_time = now;
                        cpu.hz = unsafe { HZ };
                        unsafe { HZ = current_hz };
                    } else {
                        unsafe { HZ = (cpu.hz + current_hz) / 2.0 };
                    } // Sealing the clock speed based on the clock speed/second
                    time = (cpu.clock - l_clock) as f32 / cpu.clock_speed;
                    if cpu.run_fast {
                        continue; // Skip the sleep if running at max speed
                    }
                } else {
                    time = 0.0;
                    l_clock = 0;
                }
                if time != 0.0 {
                    let sleep_interval = 0.05; // 50 milliseconds
                    let mut remaining_time = time;
                    while remaining_time > 0.0 {
                        if remaining_time < sleep_interval {
                            std::thread::sleep(std::time::Duration::from_secs_f32(remaining_time));
                            remaining_time = 0.0;
                        } else {
                            std::thread::sleep(std::time::Duration::from_secs_f32(sleep_interval));
                            remaining_time -= sleep_interval;
                        }
                        // Check if cpu.clock_speed has changed
                        if let Ok(cpu) = cpu_ref.lock() {
                            let new_time = (cpu.clock - l_clock) as f32 / cpu.clock_speed;
                            if new_time != time || cpu.run_fast {
                                break;
                            }
                        }
                    }
                }
            }
        });
    }

    fn stop_execution(&self) {
        self.running.store(false, Ordering::SeqCst);
    }
}

fn display_image_from_u32_array(
    ui: &mut egui::Ui,
    ctx: &egui::Context,
    ram: Box<[u32]>,
    st_adr: u32,
    width: u32,
    height: u32,
) {
    let mut pixels = vec![Default::default(); (width * height * 3) as usize];
    for i in 0..(width * height) {
        let pixel = ram[(st_adr + i) as usize];
        let r = ((pixel >> 16) & 0xFF) as u8;
        let g = ((pixel >> 8) & 0xFF) as u8;
        let b = (pixel & 0xFF) as u8;
        pixels[(i * 3) as usize] = r;
        pixels[(i * 3 + 1) as usize] = g;
        pixels[(i * 3 + 2) as usize] = b;
    }
    let color_image = egui::ColorImage::from_rgb([width as usize, height as usize], &pixels);
    let texture: egui::TextureHandle =
        ctx.load_texture("image", color_image, egui::TextureOptions::default());
    ui.image(&texture, [width as f32, height as f32]);
}

impl eframe::App for GUI {
    fn update(&mut self, ctx: &egui::Context, _: &mut eframe::Frame) {
        let mut cpu = self.cpu.lock().unwrap();
        let error_pcob = Arc::clone(&self.error_pc_out_of_bounds);
        let error_hlt = Arc::clone(&self.error_halt);
        // Top panel for general information and control buttons
        egui::TopBottomPanel::top("TopPanel").show(ctx, |ui| {
            ui.vertical_centered(|ui| {
                ui.heading("SS32 Emulator");
            });

            ui.vertical_centered(|ui| {
                ui.horizontal_centered(|ui| {
                    // Control Buttons Section
                    if self.running.load(Ordering::SeqCst) {
                        ui.label("Status: Running");
                        if ui.button("Stop").clicked() {
                            self.stop_execution();
                        }
                    } else {
                        ui.label("Status: Stopped");
                        if ui.button("Start").clicked() {
                            self.start_execution();
                        }
                        if ui.button("Step").clicked() {
                            cpu.execute_instruction(false, 0);
                        }
                    }
                    if ui.button("Reset").clicked() {
                        self.error_pc_out_of_bounds.store(false, Ordering::SeqCst);
                        self.error_halt.store(false, Ordering::SeqCst);
                        cpu.reset();
                    }
                    if ui.button("Restart").clicked() {
                        self.error_pc_out_of_bounds.store(false, Ordering::SeqCst);
                        self.error_halt.store(false, Ordering::SeqCst);
                        cpu.restart();
                    }
                    if ui.button("⬇ Load Ram").clicked() {
                        let path = FileDialog::new()
                            .add_filter("hex", &["hex", "hex"])
                            .pick_file();
                        if let Some(path) = path {
                            let mut initial_ram_content: Vec<u32> = Vec::new();
                            let file = File::open(&path).unwrap();
                            let reader = io::BufReader::new(file);
                            for line in reader.lines() {
                                let line = line.unwrap();
                                let number = u32::from_str_radix(&line, 16)
                                    .expect("Failed to parse hex string");
                                initial_ram_content.push(number);
                            }
                            *cpu = cpu::CPU::new(initial_ram_content, cpu.log, cpu.clock_speed);
                        }
                    }
                    ui.checkbox(&mut cpu.run_fast, "Run At Max Speed");
                    ui.add(
                        egui::Slider::new(&mut cpu.clock_speed, 0.1..=100.0).text("Clock Speed"),
                    );
                    ui.label(format!("Hz: {:.5}", cpu.hz));
                });
            });
        });

        // Left panel for the small screen
        egui::SidePanel::left("left_panel")
            .resizable(false)
            .exact_width(640.0)
            .show(ctx, |ui| {
                ui.vertical_centered(|ui| {
                    ui.heading("Screen");
                    ui.separator();
                    display_image_from_u32_array(ui, ctx, cpu.ram.clone(), 0xFB5000, 640, 480);
                    ui.separator();
                });
            });

        // Right panel for register information
        egui::SidePanel::right("right_panel")
            .resizable(false)
            .min_width(200.0)
            .show(ctx, |ui| {
                ui.heading("Registers");
                ui.separator();

                // Register Information Section
                ui.horizontal(|ui| {
                    ui.label("PC:");
                    ui.monospace(format!("0x{:08x}", cpu.registers.pc));
                });
                ui.horizontal(|ui| {
                    ui.label("SP:");
                    ui.monospace(format!("0x{:08x}", cpu.registers.sp));
                });
                ui.horizontal(|ui| {
                    ui.label("RETI:");
                    ui.monospace(format!("0x{:08x}", cpu.registers.reti));
                });
                ui.horizontal(|ui| {
                    ui.label("Privilege:");
                    ui.monospace(format!("{}", cpu.registers.privilege));
                });
                for i in 0..16 {
                    ui.horizontal(|ui| {
                        ui.label(format!("R{}:", i));
                        ui.monospace(format!("0x{:08x}", cpu.registers[i]));
                    });
                }
                ui.separator();
                if error_pcob.load(Ordering::SeqCst) {
                    ui.label("Error: Program Count Out of Bound");
                } else if error_hlt.load(Ordering::SeqCst) {
                    ui.label("Error: CPU Halted");
                } else {
                    ui.label("Error: None");
                }
            });

        // Central panel for additional information or logs if needed
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.heading("Debug Information");
            ui.separator();

            // Instruction Execution
            ui.label(format!("Current Instruction: 0x{:08x}", cpu.ir));
            ui.label(format!("Opcode: 0x{:02x}", cpu.opcode));
            ui.label(format!("DR: {}", cpu.dr));
            ui.label(format!("SR2: {}", cpu.sr2));
            ui.label(format!("SR1: {}", cpu.sr1));
            ui.label(format!("Immediate: {}", cpu.immediate));

            ui.separator();

            // CPU State
            ui.label(format!("Cycle Count: {}", cpu.clock));
            ui.label(format!(
                "Privilege Level: {}",
                if cpu.registers.privilege { 1 } else { 0 }
            ));

            // Flags
            ui.label(format!("Zero Flag: {}", cpu.registers.zero_f));
            ui.label(format!("Carry Flag: {}", cpu.registers.carry_f));
            ui.label(format!("Compare Flag: {}", cpu.registers.comp_f));

            ui.separator();

            // Memory
            ui.label("Recently Accessed Memory:");
            let (address, value) = &cpu.recent_memory_accesses;
            ui.label(format!("0x{:08x}: 0x{:08x}", address, value));

            ui.separator();
            let a_h = ui.available_height() - 20f32;
            let height = egui::TextStyle::Body.resolve(ui.style()).size;
            ui.horizontal(|ui| {
                // Stack
                ui.vertical(|ui| {
                    ui.label("Stack Contents:");
                    egui::ScrollArea::vertical()
                        .id_source("stack")
                        .min_scrolled_height(a_h)
                        .show_rows(
                            ui,
                            height,
                            cpu.ram[0xFFF400..=0xFFFFFF].len(),
                            |ui, row_range| {
                                for i in row_range {
                                    let Some(value) = cpu.ram.get(0xFFF400 + i) else {
                                        continue;
                                    };
                                    ui.label(format!("SP-{:03x}: 0x{:08x}", i, value));
                                }
                            },
                        );
                });
                ui.separator();
                // RAM
                ui.vertical(|ui| {
                    ui.label("RAM Contents:");
                    let mut search: Option<u32> = None;
                    if ui.text_edit_singleline(&mut self.search_ram).changed() {
                        search = u32::from_str_radix(&self.search_ram, 16).ok();
                        println!("Search: {:?}", self.search_ram);
                    }
                    let mut scroll_area = egui::ScrollArea::vertical();
                    let height = egui::TextStyle::Body.resolve(ui.style()).size;
                    if let Some(index) = search {
                        let offset = (height + 10f32) * index as f32;
                        println!("Offset: {}", offset);
                        scroll_area = scroll_area.vertical_scroll_offset(offset);
                    }
                    scroll_area.show_rows(ui, height, cpu.ram.len(), |ui, row_range| {
                        ui.allocate_space([ui.available_width(), 0.0].into());
                        for i in row_range {
                            let Some(value) = cpu.ram.get(i) else {
                                continue;
                            };
                            ui.label(format!("{i:06x}: {value:08x}"));
                        }
                    })
                });
            });
            ui.separator();
        });

        if self.running.load(Ordering::SeqCst) {
            ctx.request_repaint();
        }
    }
}

fn main() -> Result<(), eframe::Error> {
    let args: Vec<String> = env::args().collect();
    println!("Args: {:?}", args);

    let mut initial_ram_content: Vec<u32> = Vec::new();

    if args.len() > 1 {
        let input_path = Path::new(&args[1]); // Replace with your file path
        let file = File::open(&input_path).unwrap();
        let reader = io::BufReader::new(file);

        for line in reader.lines() {
            let line = line.unwrap();
            let number = u32::from_str_radix(&line, 16).expect("Failed to parse hex string");
            initial_ram_content.push(number);
        }
    }

    let mut log: bool = false;
    let log_file_path = if args.len() > 2 && args[2] == "-L" {
        if args.len() < 4 {
            eprintln!("Error: Log file path not provided");
            std::process::exit(1);
        }
        log = true;
        Some(&args[3])
    } else {
        None
    };

    if log {
        fast_log::init(
            fast_log::Config::new()
                .file(log_file_path.unwrap())
                .chan_len(Some(10)),
        )
        .unwrap();
    }

    let cpu = cpu::CPU::new(initial_ram_content, log, 1.0);
    // self.cpu.execute_instruction(false, 0);
    return eframe::run_native(
        "SS32",
        eframe::NativeOptions {
            ..Default::default()
        },
        Box::new(|cc| Box::new(GUI::new(cc, cpu))),
    );
}
