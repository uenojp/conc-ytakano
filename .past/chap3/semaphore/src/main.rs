mod semaphore;

use std::sync::Arc;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::thread;

use crate::semaphore::Semaphore;

const NUM_SEMS: isize = 4;
const NUM_THREADS: isize = 8;
const NUM_LOOP: isize = 10000;

static mut CNT: AtomicUsize = AtomicUsize::new(0);

fn main() {
    let mut hs = vec![];
    let sem = Arc::new(Semaphore::new(NUM_SEMS));

    for i in 0..NUM_THREADS {
        let s = Arc::clone(&sem);
        let h = thread::spawn(move || {
            for _ in 0..NUM_LOOP {
                s.wait();

                unsafe {CNT.fetch_add(1, Ordering::SeqCst)};

                let n = unsafe { CNT.load(Ordering::SeqCst) };
                println!("sem {}, CNT {}", i, n);
                assert!((n as isize) <= NUM_SEMS);

                unsafe {CNT.fetch_sub(1, Ordering::SeqCst)};

                s.post();
            }
        });
        hs.push(h);
    }

    for h in hs {
        h.join().unwrap();
    }
}
