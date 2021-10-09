use std::ptr::{read_volatile, write_volatile};
use std::thread;

const NUM_THREADS: usize = 4;
const NUM_LOOP: usize = 500000;

macro_rules! read_mem {
    ($addr: expr) => {
        unsafe { read_volatile($addr) }
    };
}

macro_rules! write_mem {
    ($addr: expr, $val: expr) => {
        unsafe { write_volatile($addr, $val) }
    };
}

struct BakeryLock {
    entering: [bool; NUM_THREADS],
    tickets: [Option<u64>; NUM_THREADS],
}

impl BakeryLock {
    fn lock(&mut self, idx: usize) -> LockGuard {
        write_mem!(&mut self.entering[idx], true);

        let mut max = 0;
        for i in 0..NUM_THREADS {
            if let Some(t) = read_mem!(&self.tickets[i]) {
                max = max.max(t);
            }
        }

        let ticket = max + 1;
        write_mem!(&mut self.tickets[idx], Some(ticket));

        write_mem!(&mut self.entering[idx], false);

        for i in 0..NUM_THREADS {
            if i == idx {
                continue;
            }
            while read_mem!(&self.entering[i]) {}

            while let Some(t) = read_mem!(&self.tickets[i]) {
                if ticket < t || (ticket == t && idx < i) {
                    break;
                }
            }
        }

        LockGuard { idx }
    }
}

struct LockGuard {
    idx: usize,
}

impl Drop for LockGuard {
    fn drop(&mut self) {
        write_mem!(&mut LOCK.tickets[self.idx], None);
    }
}

static mut LOCK: BakeryLock = BakeryLock {
    entering: [false; NUM_THREADS],
    tickets: [None; NUM_THREADS],
};

static mut COUNT: u64 = 0;

fn main() {
    let mut hs = Vec::new();

    for i in 0..NUM_THREADS {
        let h = thread::spawn(move || {
            for _ in 0..NUM_LOOP {
                let _lock = unsafe {
                    LOCK.lock(i);
                };
                let c = read_mem!(&COUNT);
                write_mem!(&mut COUNT, c + 1);
            }
        });
        hs.push(h);
    }

    for h in hs {
        h.join().unwrap();
    }
    println!("expected {}, recieved {}", NUM_LOOP * NUM_THREADS, unsafe { COUNT });
}
