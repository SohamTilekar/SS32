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

struct GUI {
    cpu: Arc<Mutex<cpu::CPU>>,
    running: Arc<AtomicBool>,
}

impl GUI {
    fn new(_cc: &eframe::CreationContext<'_>, cpu: cpu::CPU) -> Self {
        Self {
            cpu: Arc::new(Mutex::new(cpu)),
            running: Arc::new(AtomicBool::new(false)),
        }
    }

    fn start_execution(&self) {
        let cpu_ref = Arc::clone(&self.cpu);
        let running = Arc::clone(&self.running);
        running.store(true, Ordering::SeqCst);

        thread::spawn(move || {
            while running.load(Ordering::SeqCst) {
                if let Ok(mut cpu) = cpu_ref.lock() {
                    cpu.execute_instruction(false, 0);
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
        // Central panel for general information and control buttons
        let mut cpu = self.cpu.lock().unwrap();
        egui::TopBottomPanel::top("TopPanel").show(ctx, |ui| {
            ui.vertical_centered(|ui| {
                ui.heading("SS32 Emulator");
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
                    ui.horizontal_top(|ui| {
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
                            cpu.reset();
                        }
                        if ui.button("Restart").clicked() {
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
                                *cpu = cpu::CPU::new(initial_ram_content, cpu.log);
                            }
                        }
                    });
                });
                ui.separator()
            });
        // Right panel for register information
        egui::SidePanel::right("right_panel")
            // .resizable(false)
            // .min_width(200.0)
            .show(ctx, |ui| {
                ui.heading("Registers");

                ui.separator();

                // Register Information Section
                egui::ScrollArea::vertical().show(ui, |ui| {
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
                });
            });
        egui::CentralPanel::default().show(ctx, |_ui| {});
        if self.running.load(Ordering::SeqCst) {
            ctx.request_repaint();
        }
    }
}

fn main() -> Result<(), eframe::Error> {
    let args: Vec<String> = env::args().collect();
    println!("Args: {:?}", args);

    if args.len() < 2 {
        eprintln!("Usage: <executable> <file.hex> [-L <file.log>]");
        std::process::exit(1);
    }

    let input_path = Path::new(&args[1]); // Replace with your file path
    let file = File::open(&input_path).unwrap();
    let reader = io::BufReader::new(file);

    let mut initial_ram_content: Vec<u32> = Vec::new();

    for line in reader.lines() {
        let line = line.unwrap();
        let number = u32::from_str_radix(&line, 16).expect("Failed to parse hex string");
        initial_ram_content.push(number);
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

    let cpu = cpu::CPU::new(initial_ram_content, log);
    // self.cpu.execute_instruction(false, 0);
    return eframe::run_native(
        "SS32",
        eframe::NativeOptions {
            ..Default::default()
        },
        Box::new(|cc| Box::new(GUI::new(cc, cpu))),
    );
}
