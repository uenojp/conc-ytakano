struct Foo {
    val: u32,
}

fn my_func8() {
    let mut x = Foo { val: 10 };
    {
        let a = &mut x;
        println!("{}", a.val);
        // println!("{}", x.val);

        // mtable参照から immutable参照へ
        let b: &Foo = a;
        println!("{}",b.val); // ここでimmutable参照bが返却される
        a.val = 30;
    }
}
fn my_func9() {
    let mut x = Foo { val: 10 };
    {
        let a = &mut x;
        println!("{}", a.val);
        // println!("{}", x.val);

        // mutable参照からmutable参照を生成
        let b: &mut Foo = a;
        println!("{}", b.val);
        b.val = 90; // bを使うのはこれが最後で、ここで返却
        // 以降、mutable参照 aは初期状態に戻り、破壊的代入が可能となる
        println!("{}", a.val);

        a.val = 30; // aがここまで返却されないから、xはこのブロックではずっと &mut 貸与中
    }
}


fn main() {my_func8();my_func9()}
