use std::sync::{Condvar, Mutex};

pub struct Semaphore {
    mutex: Mutex<isize>,
    cond: Condvar,
    max: isize,
}

impl Semaphore {
    fn new(max: isize) -> Self {
        Semaphore {
            mutex: Mutex::new(0),
            cond: Condvar::new(),
            max,
        }
    }

    fn wait(&self) {
        let mut count = self.mutex.lock().unwrap();
        *count += 1;
        let _guard = self
            .cond
            .wait_while(count, |count| *count >= self.max)
            .unwrap();
    }

    fn post(&self) {
        let mut count = self.mutex.lock().unwrap();
        *count -= 1;
        if *count < self.max {
            self.cond.notify_one();
        }
    }
}
