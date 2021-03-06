/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/iostreams/stream.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

data_chunk valid_raw_input = to_chunk(base16_literal(
    "54b755c39207d443fd96a8d12c94446a1c6f66e39c95e894c23418d7501f681b01000"
    "0006b48304502203267910f55f2297360198fff57a3631be850965344370f732950b4"
    "7795737875022100f7da90b82d24e6e957264b17d3e5042bab8946ee5fc676d15d915"
    "da450151d36012103893d5a06201d5cf61400e96fa4a7514fc12ab45166ace618d68b"
    "8066c9c585f9ffffffff"));

BOOST_AUTO_TEST_SUITE(input_tests)

BOOST_AUTO_TEST_CASE(input__constructor_1__always__returns_default_initialized)
{
    chain::input instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(input__constructor_2__valid_input__returns_input_initialized)
{
    const chain::output_point previous_output{ null_hash, 5434u };
    chain::script script;
    BOOST_REQUIRE(script.from_data(to_chunk(base16_literal("ece424a6bb6ddf4db592c0faed60685047a361b1")), false, chain::script::parse_mode::raw_data));

    uint32_t sequence = 4568656u;

    chain::input instance(previous_output, script, sequence);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(previous_output == instance.previous_output());
    BOOST_REQUIRE(script == instance.script());
    BOOST_REQUIRE_EQUAL(sequence, instance.sequence());
}

BOOST_AUTO_TEST_CASE(input__constructor_3__valid_input__returns_input_initialized)
{
    chain::output_point previous_output{ null_hash, 5434u };
    chain::script script;
    BOOST_REQUIRE(script.from_data(to_chunk(base16_literal("ece424a6bb6ddf4db592c0faed60685047a361b1")), false, chain::script::parse_mode::raw_data));

    uint32_t sequence = 4568656u;

    auto dup_previous_output = previous_output;
    auto dup_script = script;
    chain::input instance(std::move(dup_previous_output), std::move(dup_script), sequence);

    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(previous_output == instance.previous_output());
    BOOST_REQUIRE(script == instance.script());
    BOOST_REQUIRE_EQUAL(sequence, instance.sequence());
}

BOOST_AUTO_TEST_CASE(input__constructor_4__valid_input__returns_input_initialized)
{
    chain::input expected;
    BOOST_REQUIRE(expected.from_data(valid_raw_input));

    chain::input instance(expected);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(input__constructor_5__valid_input__returns_input_initialized)
{
    chain::input expected;
    BOOST_REQUIRE(expected.from_data(valid_raw_input));

    chain::input instance(std::move(expected));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(input__from_data__insufficient_data__failure)
{
    data_chunk data(2);

    chain::input instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(input__from_data__valid_data__success)
{
    const auto junk = base16_literal("000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0");
    // data_chunk_stream_host host(junk);
    byte_source<std::array<uint8_t, 64>> source(junk);
    boost::iostreams::stream<byte_source<std::array<uint8_t, 64>>> stream(source);

    chain::input instance;
    BOOST_REQUIRE(instance.from_data(stream));
}

BOOST_AUTO_TEST_CASE(input__factory_from_data_1__valid_input__success)
{
    auto instance = chain::input::factory_from_data(valid_raw_input);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.serialized_size(), valid_raw_input.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_input.size());
    BOOST_REQUIRE(resave == valid_raw_input);
}

BOOST_AUTO_TEST_CASE(input__factory_from_data_2__valid_input__success)
{
    data_source stream(valid_raw_input);
    auto instance = chain::input::factory_from_data(stream);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.serialized_size(), valid_raw_input.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_input.size());
    BOOST_REQUIRE(resave == valid_raw_input);
}

BOOST_AUTO_TEST_CASE(input__factory_from_data_3__valid_input__success)
{
    data_source stream(valid_raw_input);
    istream_reader source(stream);
    auto instance = chain::input::factory_from_data(source);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.serialized_size(), valid_raw_input.size());

    // Re-save and compare against original.
    data_chunk resave = instance.to_data();
    BOOST_REQUIRE_EQUAL(resave.size(), valid_raw_input.size());
    BOOST_REQUIRE(resave == valid_raw_input);
}

BOOST_AUTO_TEST_CASE(input__signature_operations__bip16_inactive__returns_script_sigops)
{
    const data_chunk raw_script = to_chunk(base16_literal("02acad"));
    chain::script script;
    BOOST_REQUIRE(script.from_data(raw_script, true, chain::script::parse_mode::strict));
    chain::input instance;
    instance.set_script(script);
    BOOST_REQUIRE_EQUAL(script.sigops(false), instance.signature_operations(false));
}

BOOST_AUTO_TEST_CASE(input__signature_operations__bip16_active_cache_empty__returns_script_sigops)
{
    const data_chunk raw_script = to_chunk(base16_literal("02acad"));
    chain::script script;
    BOOST_REQUIRE(script.from_data(raw_script, true, chain::script::parse_mode::strict));
    chain::input instance;
    instance.set_script(script);
    BOOST_REQUIRE_EQUAL(script.sigops(false), instance.signature_operations(true));
}

BOOST_AUTO_TEST_CASE(input__previous_output_setter_1__roundtrip__success)
{
    const chain::output_point value
    {
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        5434u
    };

    chain::input instance;
    BOOST_REQUIRE(value != instance.previous_output());
    instance.set_previous_output(value);
    BOOST_REQUIRE(value == instance.previous_output());
    const auto& restricted = instance;
    BOOST_REQUIRE(value == restricted.previous_output());
}

BOOST_AUTO_TEST_CASE(input__previous_output_setter_2__roundtrip__success)
{
    const chain::output_point value
    {
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        5434u
    };

    auto dup_value = value;

    chain::input instance;
    BOOST_REQUIRE(value != instance.previous_output());
    instance.set_previous_output(std::move(dup_value));
    BOOST_REQUIRE(value == instance.previous_output());
    const auto& restricted = instance;
    BOOST_REQUIRE(value == restricted.previous_output());
}

BOOST_AUTO_TEST_CASE(input__script_setter_1__roundtrip__success)
{
     chain::script value;
     const auto data = to_chunk(base16_literal("ece424a6bb6ddf4db592c0faed60685047a361b1"));
     BOOST_REQUIRE(value.from_data(data, false, chain::script::parse_mode::raw_data));

    chain::input instance;
    BOOST_REQUIRE(value != instance.script());
    instance.set_script(value);
    BOOST_REQUIRE(value == instance.script());
    const auto& restricted = instance;
    BOOST_REQUIRE(value == restricted.script());
}

BOOST_AUTO_TEST_CASE(input__script_setter_2__roundtrip__success)
{
    chain::script value;
    const auto data = to_chunk(base16_literal("ece424a6bb6ddf4db592c0faed60685047a361b1"));
    BOOST_REQUIRE(value.from_data(data, false, chain::script::parse_mode::raw_data));

    auto dup_value = value;
   chain::input instance;
   BOOST_REQUIRE(value != instance.script());
   instance.set_script(std::move(dup_value));
   BOOST_REQUIRE(value == instance.script());
   const auto& restricted = instance;
   BOOST_REQUIRE(value == restricted.script());
}

BOOST_AUTO_TEST_CASE(input__sequence__roundtrip__success)
{
    uint32_t value = 1254u;
    chain::input instance;
    BOOST_REQUIRE_EQUAL(false, value == instance.sequence());
    instance.set_sequence(value);
    BOOST_REQUIRE_EQUAL(value, instance.sequence());
}

BOOST_AUTO_TEST_CASE(input__operator_assign_equals_1__always__matches_equivalent)
{
    chain::input expected;
    BOOST_REQUIRE(expected.from_data(valid_raw_input));
    chain::input instance;
    instance = chain::input::factory_from_data(valid_raw_input);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(input__operator_assign_equals_2__always__matches_equivalent)
{
    chain::input expected;
    BOOST_REQUIRE(expected.from_data(valid_raw_input));
    chain::input instance;
    instance = expected;
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(input__operator_boolean_equals__duplicates__returns_true)
{
    chain::input alpha;
    chain::input beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_input));
    BOOST_REQUIRE(beta.from_data(valid_raw_input));
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(input__operator_boolean_equals__differs__returns_false)
{
    chain::input alpha;
    chain::input beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_input));
    BOOST_REQUIRE_EQUAL(false, alpha == beta);
}

BOOST_AUTO_TEST_CASE(input__operator_boolean_not_equals__duplicates__returns_false)
{
    chain::input alpha;
    chain::input beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_input));
    BOOST_REQUIRE(beta.from_data(valid_raw_input));
    BOOST_REQUIRE_EQUAL(false, alpha != beta);
}

BOOST_AUTO_TEST_CASE(input__operator_boolean_not_equals__differs__returns_true)
{
    chain::input alpha;
    chain::input beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_input));
    BOOST_REQUIRE(alpha != beta);
}

BOOST_AUTO_TEST_SUITE_END()
