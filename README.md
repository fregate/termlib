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

	// Define order of field in Term
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

	// usage
	MyStruct s{};
	auto ec = erlterm::read_term<MyStruct>(data, s);
	if (ec) {
		// Handle error
	}

	// or

	auto s = erlterm::read_term<MyStruct>(data);
	if (s) { // check std::expected
		// use s.value()
		// else - handle error
	}

```
