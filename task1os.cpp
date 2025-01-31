#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <unordered_map>
#include <fstream>  // Added for file handling

using namespace std;

// Global environment variables
unordered_map<string, string> environ_vars = {
    {"PWD", get_current_dir_name()},
    {"PATH", getenv("PATH") ? getenv("PATH") : ""}
};

// Function prototypes
void execute_command(const vector<string>& args, bool background = false, const string& input_file = "", const string& output_file = "", bool append = false);
void change_directory(const vector<string>& args);
void list_directory(const vector<string>& args);
void list_environment_variables();
void set_environment_variable(const vector<string>& args);
void echo(const vector<string>& args);
void pause_shell();
void display_help();
void run_batch_file(const string& filename);

int main(int argc, char* argv[]) {
    if (argc > 1) {
        // Run in batch mode
        run_batch_file(argv[1]);
    } else {
        // Interactive mode
        string input;
        while (true) {
            cout << environ_vars["PWD"] << "> ";
            getline(cin, input);
            if (input.empty()) continue;

            // Parse input
            istringstream iss(input);
            vector<string> args;
            string arg;
            bool background = false;
            string input_file, output_file;
            bool append = false;

            while (iss >> arg) {
                if (arg == "<") {
                    iss >> input_file;
                } else if (arg == ">" || arg == ">>") {
                    append = (arg == ">>");
                    iss >> output_file;
                } else if (arg == "&") {
                    background = true;
                } else {
                    args.push_back(arg);
                }
            }

            if (args.empty()) continue;

            // Handle internal commands
            if (args[0] == "cd") {
                change_directory(args);
            } else if (args[0] == "dir") {
                list_directory(args);
            } else if (args[0] == "environ") {
                list_environment_variables();
            } else if (args[0] == "set") {
                set_environment_variable(args);
            } else if (args[0] == "echo") {
                echo(args);
            } else if (args[0] == "pause") {
                pause_shell();
            } else if (args[0] == "help") {
                display_help();
            } else if (args[0] == "quit") {
                break;
            } else {
                // External command
                execute_command(args, background, input_file, output_file, append);
            }
        }
    }
    return 0;
}

void execute_command(const vector<string>& args, bool background, const string& input_file, const string& output_file, bool append) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (!input_file.empty()) {
            int fd = open(input_file.c_str(), O_RDONLY);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (!output_file.empty()) {
            int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
            int fd = open(output_file.c_str(), flags, 0644);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        // Convert vector<string> to char* array
        vector<char*> argv;
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);

        execvp(argv[0], argv.data());
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        if (!background) {
            waitpid(pid, nullptr, 0);
        }
    } else {
        perror("fork");
    }
}

void change_directory(const vector<string>& args) {
    if (args.size() == 1) {
        // No directory provided, print current directory
        cout << environ_vars["PWD"] << endl;
    } else {
        if (chdir(args[1].c_str()) == 0) {
            environ_vars["PWD"] = get_current_dir_name();
        } else {
            perror("chdir");
        }
    }
}

void list_directory(const vector<string>& args) {
    string dir = args.size() > 1 ? args[1] : ".";
    DIR* dp = opendir(dir.c_str());
    if (dp == nullptr) {
        perror("opendir");
        return;
    }
    dirent* entry;
    while ((entry = readdir(dp)) != nullptr) {
        cout << entry->d_name << endl;
    }
    closedir(dp);
}

void list_environment_variables() {
    for (const auto& var : environ_vars) {
        cout << var.first << "=" << var.second << endl;
    }
}

void set_environment_variable(const vector<string>& args) {
    if (args.size() < 3) {
        cerr << "Usage: set VARIABLE VALUE" << endl;
        return;
    }
    environ_vars[args[1]] = args[2];
}

void echo(const vector<string>& args) {
    for (size_t i = 1; i < args.size(); ++i) {
        cout << args[i] << " ";
    }
    cout << endl;
}

void pause_shell() {
    cout << "Press Enter to continue...";
    cin.ignore();
}

void display_help() {
    cout << "Welcome to MyShell!\n"
         << "Supported commands:\n"
         << "  cd [DIRECTORY] - Change directory\n"
         << "  dir DIRECTORY - List directory contents\n"
         << "  environ - List environment variables\n"
         << "  set VARIABLE VALUE - Set environment variable\n"
         << "  echo [COMMENT] - Display comment\n"
         << "  pause - Pause shell\n"
         << "  help - Display this help message\n"
         << "  quit - Quit the shell\n"
         << "External commands can be executed with optional I/O redirection and background execution.\n";
}

void run_batch_file(const string& filename) {
    ifstream file(filename); // Ensure #include <fstream> is present
    if (!file.is_open()) {
        cerr << "Error: Could not open batch file " << filename << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        cout << environ_vars["PWD"] << "> " << line << endl;
        istringstream iss(line);
        vector<string> args;
        string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        if (args.empty()) continue;

        if (args[0] == "cd") {
            change_directory(args);
        } else if (args[0] == "dir") {
            list_directory(args);
        } else if (args[0] == "environ") {
            list_environment_variables();
        } else if (args[0] == "set") {
            set_environment_variable(args);
        } else if (args[0] == "echo") {
            echo(args);
        } else if (args[0] == "pause") {
            pause_shell();
        } else if (args[0] == "help") {
            display_help();
        } else if (args[0] == "quit") {
            break;
        } else {
            execute_command(args);
        }
    }
}

