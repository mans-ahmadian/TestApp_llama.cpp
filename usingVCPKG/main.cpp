#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "llama.h"  // Ensure llama.cpp headers are properly included
#ifdef _WIN32
#include <Windows.h>
#endif

int main() {
    llama_model_params model_params = llama_model_default_params();
    llama_context_params ctx_params = llama_context_default_params();

    std::string exe_dir;
#ifdef _WIN32
    char exe_path[MAX_PATH];
    if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) > 0) {
        exe_dir = std::filesystem::path(exe_path).parent_path().string();
    }
#else
    char exe_path[4096];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len > 0) {
        exe_path[len] = '\0';
        exe_dir = std::filesystem::path(exe_path).parent_path().string();
    }
#endif

    std::string model_path = exe_dir + "/models/llongma-2-7b.Q4_K_M.gguf";

    llama_model* model = llama_load_model_from_file(model_path.c_str(), model_params);
    if (!model) {
        std::cerr << "Failed to load model\n";
        return 1;
    }

    llama_context* ctx = llama_new_context_with_model(model, ctx_params);
    const llama_vocab* vocab = llama_model_get_vocab(model);

    llama_sampler_params sparams = llama_sampler_default_params();
    sparams.top_k = 40;
    sparams.top_p = 0.9f;
    sparams.temp = 0.8f;
    llama_sampler* sampler = llama_sampler_init(model, &sparams);

    std::cout << "Chat session started. Type 'exit' to quit.\n";

    while (true) {
        std::cout << "\nYou: ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") break;

        // Tokenize input
        std::vector<llama_token> tokens(input.size() + 32);
        int n_tokens = llama_tokenize(
            vocab,
            input.c_str(),
            static_cast<int>(input.length()),
            tokens.data(),
            static_cast<int>(tokens.size()),
            true,   // add_special
            false   // parse_special
        );
        if (n_tokens < 0) n_tokens = -n_tokens;
        tokens.resize(n_tokens);

        // Process input
        llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());
        llama_decode(ctx, batch);

        std::cout << "LLaMa: ";
        for (int i = 0; i < 100; ++i) {
            llama_token id = llama_sampler_sample(sampler, ctx, -1);
            if (id == llama_token_eos(vocab)) break;

            char piece[16] = { 0 };
            llama_token_to_piece(vocab, id, piece, sizeof(piece), 0, true);
            std::cout << piece << std::flush;

            llama_batch next_batch = llama_batch_get_one(&id, 1);
            llama_decode(ctx, next_batch);
        }
        std::cout << std::endl;
    }

    llama_sampler_free(sampler);
    llama_free(ctx);
    llama_free_model(model);
    return 0;
}