#include <llama.h>
#include <iostream>
#include <vector>
#include <string>
#include<filesystem>
#ifdef _WIN32
#include <windows.h>
#endif

int main() 
{
    llama_model_params model_params = llama_model_default_params();
    llama_context_params ctx_params = llama_context_default_params();

    // Find the directory of the executable
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

    // Set the model file path based on the executable directory
    std::string model_path = exe_dir + "/models/llongma-2-7b.Q4_K_M.gguf";

    llama_model* model = llama_load_model_from_file(model_path.c_str(), model_params);
    if (!model) {
        std::cerr << "Failed to load model\n";
        return 1;
    }

    llama_context* ctx = llama_new_context_with_model(model, ctx_params);
    const std::string prompt = "What is the capital of Japan?";

    // Tokenize prompt
    const llama_vocab* vocab = llama_model_get_vocab(model);
    std::vector<llama_token> tokens(prompt.size() + 32); // allocate enough space
    int n_tokens = llama_tokenize(
        vocab,
        prompt.c_str(),
        prompt.length(),
        tokens.data(),
        tokens.size(),
        true,  // add_special
        false  // parse_special
    );
    if (n_tokens < 0) n_tokens = -n_tokens;
    tokens.resize(n_tokens);

    // Prepare batch for prompt
    llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());
    llama_decode(ctx, batch);

    std::cout << "Response:\n";
    llama_sampler* sampler = llama_sampler_init_greedy();

    for (int i = 0; i < 50; ++i) {
        llama_token id = llama_sampler_sample(sampler, ctx, -1);
        char piece[16] = {0};
        llama_token_to_piece(vocab, id, piece, sizeof(piece), 0, true);
        std::cout << piece;
        llama_batch next_batch = llama_batch_get_one(&id, 1);
        llama_decode(ctx, next_batch);
    }

    llama_sampler_free(sampler);
    llama_free(ctx);
    llama_free_model(model);
    return 0;
}