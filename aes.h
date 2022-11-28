#pragma once
// this is a very thin wrapper around the gnu cryptography library
// https://www.gnupg.org/documentation/manuals/gcrypt/
// following advice from the tutorial: https://www.tnichols.org/2015/09/27/Encrypting-and-Signing-Using-libgcrypt/
// lincense: GPL-3
// Author: André Frank Krause
// email: post@andre-krause.net


#include <gcrypt.h>
#include <string>
#include <iostream>
#include <tuple>

class AES
{
private:
    std::string init_vector;
    gcry_cipher_hd_t cipher_handle;
    size_t block_length = 0;
    size_t key_length = 0;
    const size_t salt_length = 128;

    // handle errors by throwing exceptions
    void error_handler(gcry_error_t gcrypt_retval)
    {
        using namespace std;
        if(gcrypt_retval != 0)
        {
            auto error_message = string("gcrypt error: ") + gcry_strerror(gcrypt_retval);
            throw std::runtime_error(error_message);
        }
    }

    void check_data_length(const std::string& data)
    {
        if((data.size() % block_length) != 0)
        {
            auto error_message = std::string("error: data-length must be multiples of the block size: ") + std::to_string(block_length) + " bytes. You can use the AES::add_padding(string& data) function.";
            throw std::runtime_error(error_message);
        }
    }

    std::string create_random_salt()
    {
        std::string salt;
        salt.resize(salt_length,'\0');
        gcry_create_nonce(&salt[0], salt.size());
        return salt;
    }

    std::string hash_key(const std::string& key, const std::string& salt)
    {
        // https://en.wikipedia.org/wiki/PBKDF2
        // In 2021, OWASP recommended to use 310,000 iterations for PBKDF2-HMAC-SHA256 and 120,000 for PBKDF2-HMAC-SHA512
        constexpr int iterations = 500000;

        std::string kdf_key;
        kdf_key.resize(key_length);

        error_handler(gcry_kdf_derive(
            &key[0], key.size(),
            GCRY_KDF_PBKDF2, GCRY_MD_SHA512,
            &salt[0], salt.size(),
            iterations, key_length, &kdf_key[0]));
        return kdf_key;
    }

public:
    AES(bool use_secure_memory=true, bool debug=false)
    {
        const auto gcrypt_cipher = GCRY_CIPHER_AES256;   // Pick the cipher here
        const auto gcrypt_mode = GCRY_CIPHER_MODE_ECB;

        // initialize the gcrypt library - must be done before any other library function calls.
        // https://www.gnupg.org/documentation/manuals/gcrypt/Initializing-the-library.html#Initializing-the-library
        // initialization is done with gcry_check_version that also returns the version number.
        auto gcrypt_version_number = gcry_check_version(NULL);
        if(use_secure_memory)
        {
            error_handler(gcry_control(GCRYCTL_INIT_SECMEM, 32768, 0));
            error_handler(gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0));
        }

        error_handler(gcry_cipher_open(&cipher_handle, gcrypt_cipher, gcrypt_mode, 0));
        block_length = gcry_cipher_get_algo_blklen(gcrypt_cipher);
        key_length = gcry_cipher_get_algo_keylen(gcrypt_cipher);

        // create a random init vector and init the cipher.
        init_vector.resize(block_length);
        gcry_create_nonce(&init_vector[0], init_vector.size());
        error_handler(gcry_cipher_setiv(cipher_handle, &init_vector[0], block_length));

        if(debug)
        {
            std::cout << "\n== gcrypt info ==";
            std::cout << "\nversion      = " << gcrypt_version_number;
            printf(      "\ncipher mode  = %s", gcrypt_mode == GCRY_CIPHER_MODE_ECB ? "ECB" : "CBC");
            std::cout << "\nblock length = " << block_length << " bytes";
            std::cout << "\nkey length   = " << key_length  << " bytes";
            std::cout << "\ninit vector  = " << init_vector;
        }
    }

    ~AES()
    {
        gcry_cipher_close(cipher_handle);
    }

    // pad data such that modulo block_size == 0
    void add_padding(std::string& data)
    {
        if((data.size() % block_length) != 0)
        {
            int n = block_length - (data.size() % block_length);
            for(int i = 0;i<n;i++){ data.push_back(0); }
        }
    }

    std::string encrypt(const std::string& data, const std::string key)
    {
        check_data_length(data);

        auto salt = create_random_salt();
        auto kdf_key = hash_key(key, salt);
        error_handler(gcry_cipher_setkey(cipher_handle, &kdf_key[0], kdf_key.size()));

        size_t plaintxt_length = data.size();
        std::string encrypted_data;
        encrypted_data.resize(plaintxt_length);

        error_handler(
        gcry_cipher_encrypt(
            cipher_handle,
            &encrypted_data[0], plaintxt_length,
            &data[0], plaintxt_length));

      return encrypted_data + init_vector + salt;
  }

    std::string decrypt(const std::string& encrypted_data_with_init_vector_and_salt, const std::string& key)
    {
        const auto s = encrypted_data_with_init_vector_and_salt.size();
        auto salt    = encrypted_data_with_init_vector_and_salt.substr(s-salt_length, salt_length);
        auto data    = encrypted_data_with_init_vector_and_salt.substr(0, s - salt_length - block_length);
        init_vector  = encrypted_data_with_init_vector_and_salt.substr(s-salt_length-block_length-5, block_length);
        error_handler(gcry_cipher_setiv(cipher_handle, &init_vector[0], block_length));



        check_data_length(data);
        auto kdf_key = hash_key(key, salt);

        auto len = data.size();

        error_handler(gcry_cipher_setkey(cipher_handle, &kdf_key[0], kdf_key.size()));

        error_handler(
        gcry_cipher_decrypt(
            cipher_handle,
            &data[0], len,
            &data[0], len));   // size_t inlen

        return data;
    }
};


#ifdef __TEST__
int main(void)
{
    std::string plaintxt = "The quick brown fox jumps over the lazy dog.";
    std::cout <<"\nplaintext:" << plaintxt;
    std::string key = "my_fantastic_key!";
    // should throw exception regarding key lengths
    try
    {
        AES aes(true, true);
        auto encrypted = aes.encrypt(plaintxt, key);
        auto decrypted = aes.decrypt(encrypted, key);
        std::cout <<"\ndecrypted:" << decrypted;
    }
    catch(std::exception& e){ std::cerr << "\n" << e.what();}

    // should work after adding padding.
    try
    {
        AES aes(true, true);
        aes.add_padding(plaintxt);
        aes.add_padding(key);
        auto encrypted = aes.encrypt(plaintxt, key);
        auto decrypted = aes.decrypt(encrypted, key);
        std::cout <<"\ndecrypted:" << decrypted;
    }
    catch(std::exception& e){ std::cerr << "\n" << e.what();}


}
#endif
