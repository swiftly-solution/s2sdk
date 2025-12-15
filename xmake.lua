target("s2sdk")
    set_kind("static")

    set_languages("cxx23")

    add_files(
      "src/**/*.cpp"
    )

    add_includedirs(
      "public"
    )