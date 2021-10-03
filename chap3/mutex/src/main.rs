use std::sync::{Arc, Mutex};
use std::thread;

fn inc(mutex: Arc<Mutex<i32>>) {
    for _ in 0..100 {
        let mut count = mutex.lock().unwrap();
        *count += 1;
    }
}

fn main() {
    let mutex = Arc::new(Mutex::new(0));

    let mutex1 = Arc::clone(&mutex);
    let handler1 = thread::spawn(move || {
        inc(mutex1);
    });

    let mutex2 = Arc::clone(&mutex);
    let handler2 = thread::spawn(move || {
        inc(mutex2);
    });

    println!("{:?}", mutex);

    handler1.join().unwrap();
    handler2.join().unwrap();

    println!("{:?}", mutex);
}
