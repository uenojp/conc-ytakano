use std::collections::LinkedList;
use std::sync::{Arc, Condvar, Mutex};

use crate::semaphore::Semaphore;

pub fn channel<T>(max: isize) -> (Sender<T>, Receiver<T>) {
    let sem = Arc::new(Semaphore::new(max));
    let buf = Arc::new(Mutex::new(LinkedList::new()));
    let cond = Arc::new(Condvar::new());

    let tx = Sender {
        sem: Arc::clone(&sem),
        buf: Arc::clone(&buf),
        cond: Arc::clone(&cond),
    };
    let rx = Receiver { sem, buf, cond };

    (tx, rx)
}

#[derive(Clone)]
pub struct Sender<T> {
    sem: Arc<Semaphore>,
    buf: Arc<Mutex<LinkedList<T>>>,
    cond: Arc<Condvar>,
}

impl<T: Send> Sender<T> {
    pub fn send(&self, data: T) {
        self.sem.wait();
        let mut buf = self.buf.lock().unwrap();
        (*buf).push_back(data);
        self.cond.notify_one();
    }
}

pub struct Receiver<T> {
    sem: Arc<Semaphore>,
    buf: Arc<Mutex<LinkedList<T>>>,
    cond: Arc<Condvar>,
}

impl<T: Send> Receiver<T> {
    pub fn recv(&self) -> T {
        let mut buf = self.buf.lock().unwrap();
        loop {
            if let Some(data) = (*buf).pop_front() {
                self.sem.post();
                return data;
            }
            buf = self.cond.wait(buf).unwrap();
        }
    }
}
