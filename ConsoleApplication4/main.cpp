#include <Windows.h>
#include <iostream>

__declspec(noinline)void example_function() {
	std::printf("example_function()\n");
}

namespace data
{
	constexpr auto rand_val = ((__COUNTER__ * 0x10000) | 10) / 3;
	auto secret_key = _rotl64((int)(__TIME__) ^ 3, rand_val);
	auto seed_key = (int)(__TIMESTAMP__) - secret_key;

	auto dynamic_key = 0;

	auto initializate() -> void
	{
		srand(time(0));
		dynamic_key = (rand() + 0x100) ^ 6;
	}

	auto encrypt(const void* to_encrypt)  -> std::uint64_t
	{
		initializate();
		return _rotl64((unsigned long long)to_encrypt, secret_key) + seed_key + dynamic_key;
	}
	auto encrypt(unsigned long long to_encrypt)  -> std::uint64_t
	{
		initializate();
		return _rotl64((unsigned long long)to_encrypt, secret_key) + seed_key + dynamic_key;
	}

	auto decrypt(const void* to_decrypt) -> std::uint64_t
	{
		return _rotr64((unsigned long long)(to_decrypt) - seed_key - dynamic_key, secret_key);
	}
	auto decrypt(unsigned long long to_decrypt) -> std::uint64_t
	{
		return _rotr64((unsigned long long)(to_decrypt - seed_key - dynamic_key), secret_key);
	}

	template <class T>
	auto _call_function( std::uint64_t ptr ) -> void
	{
		auto fn = reinterpret_cast<T>(data::decrypt(ptr));
		fn(); // use va_list for give parameters
	}
}

static uint64_t example_fn_encrypted = data::encrypt( example_function );

void main()
{
	// std::cout << "Encrypted Fn: 0x" << std::hex << example_fn_encrypted << std::endl;
	// std::cout << "Decrypted Fn: 0x" << std::hex << data::decrypt( example_fn_encrypted ) << std::endl << std::endl;
	
	data::_call_function< decltype( &example_function ) >( example_fn_encrypted );

	std::cin.get( );
}
