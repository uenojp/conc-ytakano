use std::thread;

fn main() {
    let v = 10;
    let f = move || v * 200;

    let res = thread::spawn(f).join();
    println!("{:?}", res);

    let f = || {panic!();};
    let res = thread::spawn(f).join();
    println!("{:?}", res);
    match res {
        Ok(_) => {println!("Successed")},
        Err(e) => {
            let s = e.downcast_ref::<&str>();
            println!("{:?}", s);
        },
    };
}
