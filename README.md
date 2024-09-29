# termlib
Erlang binary term parser/builder

Lightwieght library to parse Erlang binary representation of terms (after erlang:term_to_binary) and create binary to parse with erlang:binary_to_term.

Not all Erlang types are supprted at this moment.


# Roadmap
1. Support all erlang types. And make some C++ equivalents
2. Rewrite it for look more modern, like [glaze](https://github.com/stephenberry/glaze/). For example:
```
	struct MyStruct {
		int value;
		std::string atom;
		std::string str;
		std::vector<int> single_type_list;
		std::tuple<int, std::string, double> various_types_list;
	};

	// Define order of field for list or tuple parsing
	template<>
	struct erlterm::meta<MyStruct> {
		using T = MyStruct;
		static constexpr auto value = term(
			&T::value,
			&T::str,
			&T::atom,
			&T::single_type_list,
			&T::various_types_list
		);
	};

	// Define fields name (if it has different atom name) and type for erlang map parsing
	template<>
	struct erlterm::meta<MyStruct> {
		using T = MyStruct;
		static constexpr auto value = term(
			"int_value", &T::value,
			"string_value", &T::str,
			"some_atom", &T::atom,
			"list_value", &T::single_type_list,
			"tuple_value", &T::various_types_list
		);
	};

	// data = erlang:term_to_binary([123,"abc",some_atom,[4,5,6],{7,"d",3.1415926}]).
	// <<131,108,0,0,0,5,97,123,107,0,3,97,98,99,119,9,115,111,109,101,95,97,116,111,
	//   109,107,0,3,4,5,6,104,3,97,7,107,0,1,100,70,64,9,33,251,77,18,216,74,106>>
	// usage
	MyStruct s{};
	auto ec = erlterm::read_list_term<MyStruct>(data, s); // read_tuple_term
	if (ec) {
		// Handle error
	}

	// or
	// data = erlang:term_to_binary(#{some_atom => some_atom,int_value => 123,
	//				string_value => "abc",
	//				list_value => [4,5,6],
	//				tuple_value => {7,"d",3.1415926}}).
	// <<131,116,0,0,0,5,119,9,115,111,109,101,95,97,116,111,109,119,9,115,111,109,
	//   101,95,97,116,111,109,119,9,105,110,116,95,118,97,108,117,101,97,123,119,12,
	//   115,116,114,105,110,103,95,118,97,108,117,101,107,0,3,97,98,99,119,10,108,
	//   105,115,116,95,118,97,108,117,101,107,0,3,4,5,6,119,11,116,117,112,108,101,
	//   95,118,97,108,117,101,104,3,97,7,107,0,1,100,70,64,9,33,251,77,18,216,74>>
	auto s = erlterm::read_map_term<MyStruct>(data);
	if (s) { // check std::expected
		// use s.value()
		// else - handle error
	}

```

Q: API - make sole function to parse term (read_term) or different for list/tuple/map or both?
