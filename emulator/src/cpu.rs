use rand::random;
use std::{thread, time};

#[allow(dead_code)]
struct Registers {
    r1: u32,
    r2: u32,
    r3: u32,
    r4: u32,
    r5: u32,
    r6: u32,
    r7: u32,
    r8: u32,
    r9: u32,
    r10: u32,
    r11: u32,
    r12: u32,
    r13: u32,
    r14: u32,
    tmp: u32,
    pc: usize,
    sp: u32,
    reti: u32,
    privilege: bool,
    carry_f: bool,
    zero_f: bool,
    comp_f: bool,
}

impl Registers {
    fn new() -> Registers {
        Registers {
            r1: 0,
            r2: 0,
            r3: 0,
            r4: 0,
            r5: 0,
            r6: 0,
            r7: 0,
            r8: 0,
            r9: 0,
            r10: 0,
            r11: 0,
            r12: 0,
            r13: 0,
            r14: 0,
            tmp: 0,
            pc: 0,
            sp: 0,
            reti: 0,
            privilege: false,
            carry_f: false,
            zero_f: false,
            comp_f: false,
        }
    }
}

pub(crate) struct CPU {
    registers: Registers,
    ram: Box<[u32]>, // Allocating on the heap
}

impl CPU {
    pub fn new() -> CPU {
        println!("Initializing CPU");
        println!("Allocating 64MB of RAM");
        let mut ram: Vec<u32> = Vec::with_capacity(16777216);
        let mut value: u32 = random(); // Seed value
        for _ in 0..16777216 {
            // Simple LCG formula: value = (value * 1664525 + 1013904223) % 2^32
            value = value.wrapping_mul(1664525).wrapping_add(1013904223);
            ram.push(value);
        }
        CPU {
            registers: Registers::new(),
            ram: ram.into_boxed_slice(),
        }
    }
    fn execute_instruction(&mut self) {
        // Fetch instruction from memory
        if self.registers.pc >= self.ram.len() {
            println!("PC out of bounds");
            std::process::exit(1);
        }
        let instr = self.ram[self.registers.pc];
        self.registers.pc += 1;
        // Decode and execute instruction
        if instr >> 28 == 0 {
            thread::sleep(time::Duration::from_nanos(100));
        }
    }
    pub fn run(&mut self) {
        println!("Running CPU");
        loop {
            self.execute_instruction();
        }
    }
}

impl Default for CPU {
    fn default() -> Self {
        Self::new()
    }
}
