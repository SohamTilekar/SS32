use fast_log::Config;
use std::env;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
mod cpu;

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    println!("Args: {:?}", args);

    if args.len() < 2 {
        eprintln!("Usage: <executable> <file.hex> [-L <file.log>]");
        std::process::exit(1);
    }

    let input_path = Path::new(&args[1]); // Replace with your file path
    let file = File::open(&input_path)?;
    let reader = io::BufReader::new(file);

    let mut initial_ram_content: Vec<u32> = Vec::new();

    for line in reader.lines() {
        let line = line?;
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
            Config::new()
                .file(log_file_path.unwrap())
                .chan_len(Some(10)),
        )
        .unwrap();
    }

    // Create a CPU instance
    let mut cpu = cpu::CPU::new(initial_ram_content, log);

    // Main execution loop
    loop {
        cpu.execute_instruction(false, 0);
    }
}
