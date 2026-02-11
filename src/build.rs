use std::env;
use std::path::PathBuf;
use cmake::Config;

fn main() {
    // Re-run build.rs if any of these change
    println!("cargo:rerun-if-changed=CMakeLists.txt");
    println!("cargo:rerun-if-changed=src/CMakeLists.txt");
    println!("cargo:rerun-if-changed=src/c/");
    println!("cargo:rerun-if-changed=include/");

    // Get crate root (always correct even in cargo package verification)
    let crate_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());

    // Build the native C library via CMake
    let dst = Config::new(&crate_dir)
        .build();

    // Tell Rust/Cargo where to find the static library
    println!("cargo:rustc-link-search=native={}/lib", dst.display());

    // Link the static library (without "lib" prefix or ".a" suffix)
    println!("cargo:rustc-link-lib=static=Shemodogan");
}
