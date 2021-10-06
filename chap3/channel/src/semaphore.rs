use std::sync::{Condvar, Mutex};

pub struct Semaphore {
    mutex: Mutex<isize>,
    cond: Condvar,
    max: isize,
}

impl Semaphore {
    pub fn new(max: isize) -> Self {
        Semaphore {
            mutex: Mutex::new(0),
            cond: Condvar::new(),
            max,
        }
    }

    pub fn wait(&self) {
        let mut count = self.mutex.lock().unwrap();
        count = self
            .cond
            .wait_while(count, |count| *count >= self.max)
            .unwrap();
        *count += 1;
    }

    pub fn post(&self) {
        let mut count = self.mutex.lock().unwrap();
        *count -= 1;
        if *count < self.max {
            self.cond.notify_one();
        }
    }
}
