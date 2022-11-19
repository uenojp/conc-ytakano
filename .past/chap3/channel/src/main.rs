use std::thread;

use channel::channel;

mod channel;
mod semaphore;

fn main() {
    let (tx, rx) = channel(3);

    thread::spawn(move || {
        tx.send(String::from("hi"));
        tx.send(String::from("there"));
    });

    let msg = rx.recv();
    println!("{}", msg);
    let msg = rx.recv();
    println!("{}", msg);
}
