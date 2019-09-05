#include "Jahley.h"
#include <json.hpp>

const std::string APP_NAME = "JsonTest";

#ifdef CHECK
#undef CHECK
#endif

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

using json = nlohmann::json;



TEST_CASE("[opitx] program_group")
{
	json j;
	
	j["floor_group"] = {
		{"kind", "OPTIX_PROGRAM_GROUP_KIND_HITGROUP"},
		{"moduleCH", "shading" },
		{"entryFunctionNameCH", "__closesthit__checker_radiance" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", "geometry" },
		{"entryFunctionNameIS", "__intersection__parallelogram" } };

	j["metal_sphere_group"] = {
		{"kind", "OPTIX_PROGRAM_GROUP_KIND_HITGROUP"},
		{"moduleCH", "shading" },
		{"entryFunctionNameCH", "__closesthit__metal_radiance" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", "geometry" },
		{"entryFunctionNameIS", "__intersection__sphere" } };

	j["glass_sphere_group"] = {
		{"kind", "OPTIX_PROGRAM_GROUP_KIND_HITGROUP"},
		{"moduleCH", "shading" },
		{"entryFunctionNameCH", "__closesthit__glass_radiance" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", "geometry" },
		{"entryFunctionNameIS", "__intersection__sphere_shell" } };

	j["camera_group"] = {
		{"kind", "OPTIX_PROGRAM_GROUP_KIND_RAYGEN"},
		{"module", "camera" },
		{"entryFunctionName", "__raygen__pinhole_camera" }  };

	j["miss_group"] = {
		{"kind", "OPTIX_PROGRAM_GROUP_KIND_MISS"},
		{"module", "shading" },
		{"entryFunctionName", "__miss__constant_bg" } };

	std::string path = getResourcePath(APP_NAME).toStdString();
	path += "/program_groups.json";

	std::ofstream o(path);
	o << std::setw(4) << j << std::endl;
	
	json j2;
	std::ifstream in(path);
	in >> j2;

	int count = 0;
	for (auto i : json::iterator_wrapper(j2))
	{
		++count;
		std::cout << i.key() << std::endl;
		json p = i.value();
		std::cout << p["kind"] << std::endl;
		std::cout << p["module"] << std::endl;
		std::cout << p["entryFunctionName"] << std::endl;
	}

}

// from https://github.com/nlohmann/json/tree/develop/test

TEST_CASE("[value] iterator_wrapper")
{
	json j = { {"A", 1}, {"B", 2} };
	int counter = 1;

	for (auto i : json::iterator_wrapper(j))
	{
		switch (counter++)
		{
			case 1:
			{
				CHECK(i.key() == "A");
				CHECK(i.value() == json(1));
				break;
			}

			case 2:
			{
				CHECK(i.key() == "B");
				CHECK(i.value() == json(2));
				break;
			}

			default:
			{
				break;
			}
		}
	}

	CHECK(counter == 3);

}

TEST_CASE("[reference] iterator_wrapper")
{

	json j = { {"A", 1}, {"B", 2} };
	int counter = 1;

	for (auto& i : json::iterator_wrapper(j))
	{
		switch (counter++)
		{
		case 1:
		{
			CHECK(i.key() == "A");
			CHECK(i.value() == json(1));

			// change the value
			i.value() = json(11);
			CHECK(i.value() == json(11));
			break;
		}

		case 2:
		{
			CHECK(i.key() == "B");
			CHECK(i.value() == json(2));

			// change the value
			i.value() = json(22);
			CHECK(i.value() == json(22));
			break;
		}

		default:
		{
			break;
		}
		}
	}

	CHECK(counter == 3);

	// check if values where changed
	CHECK(j == json({ {"A", 11}, {"B", 22} }));
}

TEST_CASE("[array value] iterator_wrapper")
{
	json j = { "A", "B" };
	int counter = 1;

	for (auto i : json::iterator_wrapper(j))
	{
		switch (counter++)
		{
			case 1:
			{
				CHECK(i.key() == "0");
				CHECK(i.value() == "A");
				break;
			}

			case 2:
			{
				CHECK(i.key() == "1");
				CHECK(i.value() == "B");
				break;
			}

			default:
			{
				break;
			}
		}
	}

	CHECK(counter == 3);
}

TEST_CASE("[structured_bindings] iterator_wrapper")
{
	json j = { {"A", 1}, {"B", 2} };

	std::map<std::string, int> m;

	for (auto const& [key, value] : j.items())
	{
		m.emplace(key, value);
	}

	CHECK(j.get<decltype(m)>() == m);
}

TEST_CASE("regression tests")
{
	SUBCASE("issue #60 - Double quotation mark is not parsed correctly")
	{
		SUBCASE("escape_dobulequote")
		{
			auto s = "[\"\\\"foo\\\"\"]";
			json j = json::parse(s);
			auto expected = R"(["\"foo\""])"_json;
			CHECK(j == expected);
		}
	}

	SUBCASE("issue #76 - dump() / parse() not idempotent")
	{
		// create JSON object
		json fields;
		fields["one"] = std::string("one");
		fields["two"] = std::string("two three");
		fields["three"] = std::string("three \"four\"");

		// create another JSON object by deserializing the serialization
		std::string payload = fields.dump();
		json parsed_fields = json::parse(payload);

		// check individual fields to match both objects
		CHECK(parsed_fields["one"] == fields["one"]);
		CHECK(parsed_fields["two"] == fields["two"]);
		CHECK(parsed_fields["three"] == fields["three"]);

		// check individual fields to match original input
		CHECK(parsed_fields["one"] == std::string("one"));
		CHECK(parsed_fields["two"] == std::string("two three"));
		CHECK(parsed_fields["three"] == std::string("three \"four\""));

		// check equality of the objects
		CHECK(parsed_fields == fields);

		// check equality of the serialized objects
		CHECK(fields.dump() == parsed_fields.dump());

		// check everything in one line
		CHECK(fields == json::parse(fields.dump()));
	}
}


TEST_CASE("algorithms")
{
	json j_array = { 13, 29, 3, {{"one", 1}, {"two", 2}}, true, false, {1, 2, 3}, "foo", "baz" };
	json j_object = { {"one", 1}, {"two", 2} };

	SUBCASE("non-modifying sequence operations")
	{
		SUBCASE("std::all_of")
		{
			CHECK(std::all_of(j_array.begin(), j_array.end(), [](const json& value)
				{
					return value.size() > 0;
				}));
			CHECK(std::all_of(j_object.begin(), j_object.end(), [](const json& value)
				{
					return value.type() == json::value_t::number_integer;
				}));
		}

		SUBCASE("std::any_of")
		{
			CHECK(std::any_of(j_array.begin(), j_array.end(), [](const json& value)
				{
					return value.is_string() and value.get<std::string>() == "foo";
				}));
			CHECK(std::any_of(j_object.begin(), j_object.end(), [](const json& value)
				{
					return value.get<int>() > 1;
				}));
		}

		SUBCASE("std::none_of")
		{
			CHECK(std::none_of(j_array.begin(), j_array.end(), [](const json& value)
				{
					return value.size() == 0;
				}));
			CHECK(std::none_of(j_object.begin(), j_object.end(), [](const json& value)
				{
					return value.get<int>() <= 0;
				}));
		}

		SUBCASE("std::for_each")
		{
			SUBCASE("reading")
			{
				int sum = 0;

				std::for_each(j_array.cbegin(), j_array.cend(), [&sum](const json& value)
					{
						if (value.is_number())
						{
							sum += static_cast<int>(value);
						}
					});

				CHECK(sum == 45);
			}

			SUBCASE("writing")
			{
				auto add17 = [](json& value)
				{
					if (value.is_array())
					{
						value.push_back(17);
					}
				};

				std::for_each(j_array.begin(), j_array.end(), add17);

				CHECK(j_array[6] == json({ 1, 2, 3, 17 }));
			}
		}

		SUBCASE("std::count")
		{
			CHECK(std::count(j_array.begin(), j_array.end(), json(true)) == 1);
		}

		SUBCASE("std::count_if")
		{
			CHECK(std::count_if(j_array.begin(), j_array.end(), [](const json& value)
				{
					return (value.is_number());
				}) == 3);
			CHECK(std::count_if(j_array.begin(), j_array.end(), [](const json&)
				{
					return true;
				}) == 9);
		}

		SUBCASE("std::mismatch")
		{
			json j_array2 = { 13, 29, 3, {{"one", 1}, {"two", 2}, {"three", 3}}, true, false, {1, 2, 3}, "foo", "baz" };
			auto res = std::mismatch(j_array.begin(), j_array.end(), j_array2.begin());
			CHECK(*res.first == json({ {"one", 1}, {"two", 2} }));
			CHECK(*res.second == json({ {"one", 1}, {"two", 2}, {"three", 3} }));
		}

		SUBCASE("std::equal")
		{
			SUBCASE("using operator==")
			{
				CHECK(std::equal(j_array.begin(), j_array.end(), j_array.begin()));
				CHECK(std::equal(j_object.begin(), j_object.end(), j_object.begin()));
				CHECK(not std::equal(j_array.begin(), j_array.end(), j_object.begin()));
			}

			SUBCASE("using user-defined comparison")
			{
				// compare objects only by size of its elements
				json j_array2 = { 13, 29, 3, {"Hello", "World"}, true, false, {{"one", 1}, {"two", 2}, {"three", 3}}, "foo", "baz" };
				CHECK(not std::equal(j_array.begin(), j_array.end(), j_array2.begin()));
				CHECK(std::equal(j_array.begin(), j_array.end(), j_array2.begin(),
					[](const json& a, const json& b)
					{
						return (a.size() == b.size());
					}));
			}
		}

		SUBCASE("std::find")
		{
			auto it = std::find(j_array.begin(), j_array.end(), json(false));
			CHECK(std::distance(j_array.begin(), it) == 5);
		}

		SUBCASE("std::find_if")
		{
			auto it = std::find_if(j_array.begin(), j_array.end(),
				[](const json& value)
				{
					return value.is_boolean();
				});
			CHECK(std::distance(j_array.begin(), it) == 4);
		}

		SUBCASE("std::find_if_not")
		{
			auto it = std::find_if_not(j_array.begin(), j_array.end(),
				[](const json& value)
				{
					return value.is_number();
				});
			CHECK(std::distance(j_array.begin(), it) == 3);
		}

		SUBCASE("std::adjacent_find")
		{
			CHECK(std::adjacent_find(j_array.begin(), j_array.end()) == j_array.end());
			CHECK(std::adjacent_find(j_array.begin(), j_array.end(),
				[](const json& v1, const json& v2)
				{
					return v1.type() == v2.type();
				}) == j_array.begin());
		}
	}

	SUBCASE("modifying sequence operations")
	{
		SUBCASE("std::reverse")
		{
			std::reverse(j_array.begin(), j_array.end());
			CHECK(j_array == json({ "baz", "foo", {1, 2, 3}, false, true, {{"one", 1}, {"two", 2}}, 3, 29, 13 }));
		}

		SUBCASE("std::rotate")
		{
			std::rotate(j_array.begin(), j_array.begin() + 1, j_array.end());
			CHECK(j_array == json({ 29, 3, {{"one", 1}, {"two", 2}}, true, false, {1, 2, 3}, "foo", "baz", 13 }));
		}

		SUBCASE("std::partition")
		{
			auto it = std::partition(j_array.begin(), j_array.end(), [](const json& v)
				{
					return v.is_string();
				});
			CHECK(std::distance(j_array.begin(), it) == 2);
			CHECK(not it[2].is_string());
		}
	}

	SUBCASE("sorting operations")
	{
		SUBCASE("std::sort")
		{
			SUBCASE("with standard comparison")
			{
				json j = { 13, 29, 3, {{"one", 1}, {"two", 2}}, true, false, {1, 2, 3}, "foo", "baz", nullptr };
				std::sort(j.begin(), j.end());
				CHECK(j == json({ nullptr, false, true, 3, 13, 29, {{"one", 1}, {"two", 2}}, {1, 2, 3}, "baz", "foo" }));
			}

			SUBCASE("with user-defined comparison")
			{
				json j = { 3, {{"one", 1}, {"two", 2}}, {1, 2, 3}, nullptr };
				std::sort(j.begin(), j.end(), [](const json& a, const json& b)
					{
						return a.size() < b.size();
					});
				CHECK(j == json({ nullptr, 3, {{"one", 1}, {"two", 2}}, {1, 2, 3} }));
			}

			SUBCASE("sorting an object")
			{
				json j({ {"one", 1}, {"two", 2} });
				CHECK_THROWS_AS(std::sort(j.begin(), j.end()), json::invalid_iterator&);
				/*CHECK_THROWS_WITH(std::sort(j.begin(), j.end()),
					"[json.exception.invalid_iterator.209] cannot use offsets with object iterators");*/
			}
		}

		SUBCASE("std::partial_sort")
		{
			json j = { 13, 29, 3, {{"one", 1}, {"two", 2}}, true, false, {1, 2, 3}, "foo", "baz", nullptr };
			std::partial_sort(j.begin(), j.begin() + 4, j.end());
			CHECK(j == json({ nullptr, false, true, 3, {{"one", 1}, {"two", 2}}, 29, {1, 2, 3}, "foo", "baz", 13 }));
		}
	}

	SUBCASE("set operations")
	{
		SUBCASE("std::merge")
		{
			{
				json j1 = { 2, 4, 6, 8 };
				json j2 = { 1, 2, 3, 5, 7 };
				json j3;

				std::merge(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
				CHECK(j3 == json({ 1, 2, 2, 3, 4, 5, 6, 7, 8 }));
			}
		}

		SUBCASE("std::set_difference")
		{
			json j1 = { 1, 2, 3, 4, 5, 6, 7, 8 };
			json j2 = { 1, 2, 3, 5, 7 };
			json j3;

			std::set_difference(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
			CHECK(j3 == json({ 4, 6, 8 }));
		}

		SUBCASE("std::set_intersection")
		{
			json j1 = { 1, 2, 3, 4, 5, 6, 7, 8 };
			json j2 = { 1, 2, 3, 5, 7 };
			json j3;

			std::set_intersection(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
			CHECK(j3 == json({ 1, 2, 3, 5, 7 }));
		}

		SUBCASE("std::set_union")
		{
			json j1 = { 2, 4, 6, 8 };
			json j2 = { 1, 2, 3, 5, 7 };
			json j3;

			std::set_union(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
			CHECK(j3 == json({ 1, 2, 3, 4, 5, 6, 7, 8 }));
		}

		SUBCASE("std::set_symmetric_difference")
		{
			json j1 = { 2, 4, 6, 8 };
			json j2 = { 1, 2, 3, 5, 7 };
			json j3;

			std::set_symmetric_difference(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
			CHECK(j3 == json({ 1, 3, 4, 5, 6, 7, 8 }));
		}
	}

	SUBCASE("heap operations")
	{
		std::make_heap(j_array.begin(), j_array.end());
		CHECK(std::is_heap(j_array.begin(), j_array.end()));
		std::sort_heap(j_array.begin(), j_array.end());
		CHECK(j_array == json({ false, true, 3, 13, 29, {{"one", 1}, {"two", 2}}, {1, 2, 3}, "baz", "foo" }));
	}
}

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		doctest::Context().run();
	}

  private:
	
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}
