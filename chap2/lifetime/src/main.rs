struct Foo {
    val: u32,
}

fn add<'a>(x: &'a Foo, y: &'a Foo) -> u32 {
    x.val + y.val
}
fn main() {
    let mut x = Foo { val: 10 };
    {
        let y = Foo { val: 20 };
        let z = add(&x, &y);
        println!("{}", z);
    }

    {
        let a = &mut x;
        println!("{}", a.val);
        println!("{}", x.val);
    }
}
