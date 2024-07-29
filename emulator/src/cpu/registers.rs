use std::ops::{Index, IndexMut};

pub struct Registers {
    pub r0: u32,
    pub r1: u32,
    pub r2: u32,
    pub r3: u32,
    pub r4: u32,
    pub r5: u32,
    pub r6: u32,
    pub r7: u32,
    pub r8: u32,
    pub r9: u32,
    pub r10: u32,
    pub r11: u32,
    pub r12: u32,
    pub r13: u32,
    pub r14: u32,
    pub tmp: u32,
    pub pc: usize,
    pub sp: u32,
    pub reti: u32,
    pub privilege: bool,
    pub carry_f: bool,
    pub zero_f: bool,
    pub comp_f: bool,
}

impl Registers {
    pub fn new() -> Registers {
        Registers {
            r0: 0,
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
    pub fn increment_sp(&mut self) {
        self.sp += 1;
    }
    pub fn decrement_sp(&mut self) {
        self.sp -= 1;
    }
}

impl Index<usize> for Registers {
    type Output = u32;
    fn index(&self, index: usize) -> &Self::Output {
        match index {
            0 => &self.r0,
            1 => &self.r1,
            2 => &self.r2,
            3 => &self.r3,
            4 => &self.r4,
            5 => &self.r5,
            6 => &self.r6,
            7 => &self.r7,
            8 => &self.r8,
            9 => &self.r9,
            10 => &self.r10,
            11 => &self.r11,
            12 => &self.r12,
            13 => &self.r13,
            14 => &self.r14,
            15 => &self.tmp,
            _ => panic!("Index out of range"),
        }
    }
}

impl IndexMut<usize> for Registers {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        match index {
            0 => &mut self.r0,
            1 => &mut self.r1,
            2 => &mut self.r2,
            3 => &mut self.r3,
            4 => &mut self.r4,
            5 => &mut self.r5,
            6 => &mut self.r6,
            7 => &mut self.r7,
            8 => &mut self.r8,
            9 => &mut self.r9,
            10 => &mut self.r10,
            11 => &mut self.r11,
            12 => &mut self.r12,
            13 => &mut self.r13,
            14 => &mut self.r14,
            15 => &mut self.tmp,
            _ => panic!("Index out of range"),
        }
    }
}
