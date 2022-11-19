use std::sync::Arc;
use std::sync::Condvar;
use std::sync::Mutex;
use std::thread;

fn child(id: u64, p: Arc<(Mutex<bool>, Condvar)>) {
    let &(ref lock, ref cvar) = &*p;

    let mut started = lock.lock().unwrap();
    while !*started {
        started = cvar.wait(started).unwrap();
    }
    // cvar.wait_while(started, |started| !*started);
    println!("child id: {}", id);
}

fn parent(p: Arc<(Mutex<bool>, Condvar)>) {
    let &(ref lock, ref cvar) = &*p;

    let mut started = lock.lock().unwrap();
    *started = true;
    cvar.notify_all();
    println!("parent");
}

fn main() {
    let pair = Arc::new((Mutex::new(false), Condvar::new()));
    let pair1 = Arc::clone(&pair);
    let pair2 = Arc::clone(&pair);

    let h1 = thread::spawn(move || child(1, pair1));
    let h2 = thread::spawn(move || child(2, pair2));

    /* sleepによりparentがnotiryするまでchildが起きないことがわかる */
    thread::sleep(std::time::Duration::from_millis(10));

    let h = thread::spawn(move || parent(pair));

    h1.join().unwrap();
    h2.join().unwrap();
    h.join().unwrap();
}
