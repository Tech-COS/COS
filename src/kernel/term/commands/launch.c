#include "kernel/cos_filesystem.h"
#include "kernel/cos_memory.h"
#include "kernel/term/tty.h"
#include "../../../../MemoryManagement/include/cos_memory_management.h"
#include "../../../../FileSystem/include/stat.h"
#include "kernel/term/commands.h"
#include "kernel/lib/string.h"

extern BinFileData_t *(*check_windows_type(uint8_t *binary))(uint8_t *);
extern BinFileData_t *windows32_fill_bin_file_data(uint8_t *binary);
extern BinFileData_t *windows64_fill_bin_file_data(uint8_t *binary);
extern BinFileData_t *linux64_fill_bin_file_data(uint8_t *binary);

extern void launch_new_user_process(const BinFileData_t *bin_file_data, uint8_t *binary, char *proc_command);
extern int cos_stat(const char *path, stat_t *statbuf);

static void free_bin_file_data(BinFileData_t *bin_file_data)
{
    if (!bin_file_data)
        return;
    if (bin_file_data->imported_libraries)
    {
        for (uint64_t i = 0; bin_file_data->imported_libraries[i].first_thunk; ++i)
        {
            if (bin_file_data->imported_libraries[i].required_functions)
            {
                cos_free(bin_file_data->imported_libraries[i].required_functions);
            }
        }
        cos_free(bin_file_data->imported_libraries);
    }
    if (bin_file_data->sections)
        cos_free(bin_file_data->sections);
    cos_free(bin_file_data);
}

void cos_launch_command(int32_t argc, char argv[32][20])
{
    if (argc < 2) {
        cos_printf("Only one parameter is required\n");
        return;
    }

    stat_t file_info;
    if (cos_stat(argv[1], &file_info))
        return;
    uint8_t *content = cos_malloc(file_info.st_size);

    if (read(argv[1], content, file_info.st_size, 0)) {
        BinFileData_t *(* handler)(uint8_t *) = check_windows_type(content);
        BinFileData_t *bin_file_data = NULL;

        if (handler == NULL)
            bin_file_data = linux64_fill_bin_file_data(content);
        else
            bin_file_data = handler(content);

        if (!bin_file_data)
            return;

        char command[20 * 32 + 1] = {0};
        memset(command, 0, 20 * 32 + 1);
        cos_memcpy(&command[0], argv[0], cos_strlen(argv[0]));
        command[cos_strlen(argv[0])] = ' ';
        cos_memcpy(&command[cos_strlen(argv[0]) + 1], argv[1], cos_strlen(argv[1]));
        command[cos_strlen(argv[0]) + cos_strlen(argv[1]) + 1] = '\0';
        launch_new_user_process(bin_file_data, content, command);
        free_bin_file_data(bin_file_data);

    } else
        cos_printf("Failed to read the file\n");

    cos_free(content);
}
