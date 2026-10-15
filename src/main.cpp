#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ir/IRGenerator.h"
#include "optimizer/Optimizer.h"
#include "codegen/CodeGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace miniopt;

struct ProgramOptions {
    std::string inputFile;
    bool showTokens = false;
    bool showAST = false;
    bool showIR = false;
    bool showOptimizedIR = false;
    bool showAssembly = false;
    bool runBenchmark = false;
    bool showHelp = false;

    OptimizerOptions optOptions;
};

void printUsage(const char* progName) {
    std::cout << "MiniOpt — Mini Compiler & Code Optimizer\n";
    std::cout << "Usage: " << progName << " <input_file> [options]\n\n";
    std::cout << "Stages / Output Options:\n";
    std::cout << "  --tokens          Print token stream and exit\n";
    std::cout << "  --ast             Print Abstract Syntax Tree and exit\n";
    std::cout << "  --ir              Print Three Address Code (TAC) and exit\n";
    std::cout << "  --optimized-ir    Print optimized TAC and exit\n";
    std::cout << "  --assembly        Print generated Pseudo-Assembly and exit\n";
    std::cout << "  --benchmark       Run optimization benchmarks across examples\n";
    std::cout << "  -h, --help        Show this help message\n\n";
    std::cout << "Optimization Flags:\n";
    std::cout << "  --no-const-fold   Disable Constant Folding\n";
    std::cout << "  --no-const-prop   Disable Constant Propagation\n";
    std::cout << "  --no-algebraic    Disable Algebraic Simplification\n";
    std::cout << "  --no-cse          Disable Common Subexpression Elimination\n";
    std::cout << "  --no-dce          Disable Dead Code Elimination\n";
}

bool parseArguments(int argc, char* argv[], ProgramOptions& opts) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            opts.showHelp = true;
            return true;
        } else if (arg == "--tokens") {
            opts.showTokens = true;
        } else if (arg == "--ast") {
            opts.showAST = true;
        } else if (arg == "--ir") {
            opts.showIR = true;
        } else if (arg == "--optimized-ir") {
            opts.showOptimizedIR = true;
        } else if (arg == "--assembly") {
            opts.showAssembly = true;
        } else if (arg == "--benchmark") {
            opts.runBenchmark = true;
        } else if (arg == "--no-const-fold") {
            opts.optOptions.enableConstantFolding = false;
        } else if (arg == "--no-const-prop") {
            opts.optOptions.enableConstantPropagation = false;
        } else if (arg == "--no-algebraic") {
            opts.optOptions.enableAlgebraicSimplification = false;
        } else if (arg == "--no-cse") {
            opts.optOptions.enableCSE = false;
        } else if (arg == "--no-dce") {
            opts.optOptions.enableDeadCodeElimination = false;
        } else if (arg.rfind("-", 0) == 0) {
            std::cerr << "Unknown option: " << arg << "\n";
            return false;
        } else {
            opts.inputFile = arg;
        }
    }
    return true;
}

std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void runBenchmarkSuite() {
    std::vector<std::pair<std::string, std::string>> benchmarks = {
        {"basic.txt", "examples/basic.txt"},
        {"constant_folding.txt", "examples/constant_folding.txt"},
        {"dead_code.txt", "examples/dead_code.txt"},
        {"complex.txt", "examples/complex.txt"}
    };

    std::cout << "====================================================================\n";
    std::cout << "                    MINIOPT BENCHMARK SUITE                         \n";
    std::cout << "====================================================================\n";
    std::cout << std::left << std::setw(24) << "Benchmark"
              << std::right << std::setw(10) << "Before"
              << std::setw(10) << "After"
              << std::setw(12) << "Removed"
              << std::setw(14) << "Reduction" << "\n";
    std::cout << "--------------------------------------------------------------------\n";

    for (const auto& bm : benchmarks) {
        std::string source;
        try {
            source = readFile(bm.second);
        } catch (...) {
            // Try relative path from build folder
            try {
                source = readFile("../" + bm.second);
            } catch (...) {
                std::cerr << "Skipping " << bm.first << " (file not found)\n";
                continue;
            }
        }

        Lexer lexer(source);
        Parser parser(lexer.tokenize());
        auto program = parser.parse();

        IRGenerator irGen;
        auto tac = irGen.generate(*program);

        Optimizer optimizer;
        auto optTac = optimizer.optimize(tac);

        size_t beforeCount = tac.size();
        size_t afterCount = optTac.size();
        size_t removed = (beforeCount >= afterCount) ? (beforeCount - afterCount) : 0;
        double reduction = (beforeCount > 0) ? (static_cast<double>(removed) / beforeCount * 100.0) : 0.0;

        std::cout << std::left << std::setw(24) << bm.first
                  << std::right << std::setw(10) << beforeCount
                  << std::setw(10) << afterCount
                  << std::setw(12) << removed
                  << std::fixed << std::setprecision(1) << std::setw(13) << reduction << "%\n";
    }
    std::cout << "====================================================================\n";
}

int main(int argc, char* argv[]) {
    ProgramOptions opts;
    if (!parseArguments(argc, argv, opts)) {
        printUsage(argv[0]);
        return 1;
    }

    if (opts.showHelp) {
        printUsage(argv[0]);
        return 0;
    }

    if (opts.runBenchmark) {
        runBenchmarkSuite();
        return 0;
    }

    if (opts.inputFile.empty()) {
        std::cerr << "Error: No input file specified.\n\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string source;
    try {
        source = readFile(opts.inputFile);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    try {
        // 1. Lexical Analysis
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        if (opts.showTokens) {
            std::cout << "========== TOKENS ==========\n";
            for (const auto& token : tokens) {
                std::cout << std::left << std::setw(16) << tokenTypeToString(token.type)
                          << token.lexeme << "\n";
            }
            return 0;
        }

        // 2. Parsing & AST
        Parser parser(tokens);
        auto ast = parser.parse();

        if (opts.showAST) {
            std::cout << "========== AST ==========\n";
            ast->print(std::cout);
            return 0;
        }

        // 3. IR Generation (Three Address Code)
        IRGenerator irGen;
        auto tac = irGen.generate(*ast);

        if (opts.showIR) {
            std::cout << "========== THREE ADDRESS CODE ==========\n";
            tac.print(std::cout);
            return 0;
        }

        // 4. Optimization Passes
        Optimizer optimizer(opts.optOptions);
        auto optimizedTac = optimizer.optimize(tac);

        if (opts.showOptimizedIR) {
            std::cout << "========== OPTIMIZED TAC ==========\n";
            optimizedTac.print(std::cout);
            return 0;
        }

        // 5. Code Generation (Pseudo-Assembly)
        CodeGenerator codeGen;
        auto assembly = codeGen.generateString(optimizedTac);

        if (opts.showAssembly) {
            std::cout << "========== PSEUDO ASSEMBLY ==========\n";
            std::cout << assembly;
            return 0;
        }

        // Default Full Pipeline Output
        std::cout << "========== SOURCE ==========\n";
        std::cout << source;
        if (!source.empty() && source.back() != '\n') std::cout << "\n";

        std::cout << "\n========== TOKENS ==========\n";
        for (const auto& token : tokens) {
            if (token.type == TokenType::END_OF_FILE) continue;
            std::cout << std::left << std::setw(16) << tokenTypeToString(token.type)
                      << token.lexeme << "\n";
        }

        std::cout << "\n========== AST ==========\n";
        ast->print(std::cout);

        std::cout << "\n========== THREE ADDRESS CODE ==========\n";
        tac.print(std::cout);

        std::cout << "\n========== OPTIMIZATION ==========\n";
        std::cout << "Constant Folding:        " << (opts.optOptions.enableConstantFolding ? "ON" : "OFF") << "\n";
        std::cout << "Constant Propagation:    " << (opts.optOptions.enableConstantPropagation ? "ON" : "OFF") << "\n";
        std::cout << "Algebraic Simplification: " << (opts.optOptions.enableAlgebraicSimplification ? "ON" : "OFF") << "\n";
        std::cout << "CSE:                     " << (opts.optOptions.enableCSE ? "ON" : "OFF") << "\n";
        std::cout << "Dead Code Elimination:   " << (opts.optOptions.enableDeadCodeElimination ? "ON" : "OFF") << "\n";

        std::cout << "\n========== OPTIMIZED TAC ==========\n";
        optimizedTac.print(std::cout);

        std::cout << "\n========== PSEUDO ASSEMBLY ==========\n";
        std::cout << assembly;

        std::cout << "\n========== STATISTICS ==========\n";
        size_t originalCount = tac.size();
        size_t optCount = optimizedTac.size();
        size_t removed = (originalCount >= optCount) ? (originalCount - optCount) : 0;
        double reduction = (originalCount > 0) ? (static_cast<double>(removed) / originalCount * 100.0) : 0.0;

        std::cout << "Original instructions:  " << originalCount << "\n";
        std::cout << "Optimized instructions: " << optCount << "\n";
        std::cout << "Instructions removed:   " << removed << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Reduction:              " << reduction << "%\n";

    } catch (const LexerError& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (const ParserError& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (const IRError& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
