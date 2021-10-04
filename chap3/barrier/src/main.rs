use std::sync::{Arc, Barrier};
use std::thread;

fn main() {
    let barrier = Arc::new(Barrier::new(10));
    let mut handlers = vec![];

    for _ in 0..10 {
        let b = Arc::clone(&barrier);
        let h = thread::spawn(move || {
            println!("waiting...");
            b.wait();
            println!("ready");
        });
        handlers.push(h);
    }

    for h in handlers {
        h.join().unwrap();
    }
}
