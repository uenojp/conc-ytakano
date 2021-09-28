fn mul_x(x: u64) -> Box<dyn Fn(u64) -> u64> {
    // xはu64であり、Copyトレイとを実装しているからmove必要ないように見える
    // がしかし、コンパイルエラー
    // > closure may outlive the current function, but it borrows `x`,
    // > which is owned by the current function
    // Boxはヒープ確保するから、自由変数xはヒープ上(これは関係なさそう)
    // 本には、p38
    // > 参照を取得した場合、変数は数mul_xから抜けた時点で
    // > 破棄されてしまい無効な参照となってしまうからである。
    // とある
    // まあ半分納得?
    // fn f(x: u64) {&x} みたいな例では、xは関数から抜けると無効な参照となる(コンパイルさせてくれない)
    // copyできる型はmoveが一切必要ないという意識を消すべきか

    // moveはxの所有権がmul_xからヒープ上のクロージャへ映る
    Box::new(move |y| x * y)
}
fn main() {
    let x = 200;
    let f = move || x * 2;
    // let f = move || x * 2; // こちらも有効
    println!("{}", f());
    println!("{}", x); // xはu64であり、Copyトレイトを実装しているのでmoveされていない
}
