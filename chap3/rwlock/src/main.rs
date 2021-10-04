use std::sync::{Arc, RwLock};
use std::thread;

fn reader(lock: Arc<RwLock<i32>>) {
    let count = lock.read().unwrap();
    println!("in reader {}", count);
}

fn writer(lock: Arc<RwLock<i32>>) {
    let mut count = lock.write().unwrap();
    *count += 1;

    println!("in writer {}", count);
}

fn main() {
    let counter =  Arc::new(RwLock::new(0));
    let mut rhandlers = vec![];
    let mut whandlers = vec![];
    for _ in 0..10 {
        let counter = Arc::clone(&counter);
        let h = thread::spawn(|| {
            reader(counter);
        });
        rhandlers.push(h);
    }

    for _ in 0..10 {
        let counter = Arc::clone(&counter);
        let h = thread::spawn(|| {
            writer(counter);
        });
        whandlers.push(h);
    }

    for h in rhandlers {
        h.join().unwrap();
    }
    for h in whandlers {
        h.join().unwrap();
    }

    print!("{:?}", counter);
}
