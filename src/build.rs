fn main() {
    // Re-run build.rs if any of these change
    println!("cargo:rerun-if-changed=CMakeLists.txt");
    println!("cargo:rerun-if-changed=src/CMakeLists.txt");

    // Build the native C library via CMake
    let dst = cmake::Config::new(".")
        .build();

    // Tell Cargo where to find the static library
    println!(
        "cargo:rustc-link-search=native={}/lib",
        dst.display()
    );

    // Link the static library
    println!("cargo:rustc-link-lib=static=shd");

    // ---- Generate Rust bindings via bindgen ----
    // let bindings = bindgen::Builder::default()
    //     .header("include/shd.h") // main header with handler APIs
    //     .clang_arg("-Iinclude")
    //     .parse_callbacks(Box::new(bindgen::CargoCallbacks))
    //     .generate()
    //     .expect("Unable to generate bindings");
    // 
    // let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    // bindings
    //     .write_to_file(out_path.join("bindings.rs"))
    //     .expect("Couldn't write bindings!");
}