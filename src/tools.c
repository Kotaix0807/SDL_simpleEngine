/**
 * @file tools.c
 * @brief Implementacion de funciones de utilidad general para el motor SDL.
 *
 * Contiene algoritmos, manejo de archivos y directorios, funciones de debug,
 * utilidades SDL, utilidades de strings y lectura de archivos de texto.
 */

// ============================================================
// Includes
// ============================================================

#include <stdio.h>
#define _POSIX_C_SOURCE 200809L

#include "config.h"
#include "tools.h"

static FILE *logFile = NULL;

// ============================================================
// Algoritmos
// ============================================================

/** @brief Busqueda binaria recursiva sobre un arreglo de enteros ordenado. */
int recBinarySearch(int arr[], int left, int right, int key)
{
    if (right >= left)
    {
        int mid = left + (right - left) / 2;

        if (arr[mid] == key)
            return mid;

        if (arr[mid] > key)
            return recBinarySearch(arr, left, mid - 1, key);

        return recBinarySearch(arr, mid + 1, right, key);
    }
    return -1;
}

// ============================================================
// Sistema de archivos
// ============================================================

/** @brief Verifica si un directorio existe. */
bool DirExists(const char *path)
{
    struct stat stats;
    stat(path, &stats);
    if (S_ISDIR(stats.st_mode))
        return true;
    return false;
}

/** @brief Cuenta la cantidad de archivos en un directorio. */
int filesInDir(char *path)
{
    DIR *dr = opendir(path);
    struct dirent *de;
    if (dr == NULL)
    {
        printDebug(LOG_ERROR, "No se pudo abirir '%s'\n", path);
        return -1;
    }

    int count = 0;
    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
            count++;
        #ifdef TOOLS_DEBUG
        printf("[ARCHIVOS DETECTADOS]\n");
        printf("%s\n", de->d_name);
        #endif
    }
    closedir(dr);
    return count;
}

/** @brief Verifica si un nombre de archivo tiene una extension valida. */
static int hasValidExtension(const char *filename, const char **extensions, int extCount, valid_type type)
{
    if (extensions == NULL || extCount <= 0)
        return 1;

    const char *ext = strrchr(filename, '.');
    if (ext == NULL)
        return 0;

    for (int i = 0; i < extCount; i++)
    {
        if (strcmp(ext, extensions[i]) == 0)
            return 1;
    }
    printDebug(LOG_WARN, "El archivo '%s' no es %s\n", filename, typeAdmited(type));
    return 0;
}

/** @brief Obtiene un arreglo con los nombres de archivo filtrados por extension. */
char **getFilesFromDir(char *path, int *outCount, const char **extensions, int extCount, valid_type type)
{
    DIR *dr = opendir(path);
    struct dirent *de;
    if (dr == NULL)
    {
        printDebug(LOG_ERROR, "No se pudo abrir '%s'\n", path);
        if (outCount)
            *outCount = 0;
        return NULL;
    }

    int capacity = 16;
    int count = 0;
    char **fileArray = malloc(sizeof(char *) * capacity);
    if (fileArray == NULL)
    {
        printDebug(LOG_ERROR, "Error al asignar memoria\n");
        closedir(dr);
        if (outCount)
            *outCount = 0;
        return NULL;
    }

    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 &&
            hasValidExtension(de->d_name, extensions, extCount, type))
        {
            if (count >= capacity)
            {
                capacity *= 2;
                char **tmp = realloc(fileArray, sizeof(char *) * capacity);
                if (tmp == NULL)
                {
                    printDebug(LOG_ERROR, "Error al reasignar memoria\n");
                    freeStringArray(fileArray, count);
                    closedir(dr);
                    if (outCount)
                        *outCount = 0;
                    return NULL;
                }
                fileArray = tmp;
            }

            fileArray[count] = strdup(de->d_name);
            if (fileArray[count] == NULL)
            {
                printDebug(LOG_ERROR, "Error al copiar nombre de archivo\n");
                for (int i = 0; i < count; i++)
                    free(fileArray[i]);
                free(fileArray);
                closedir(dr);
                if (outCount)
                    *outCount = 0;
                return NULL;
            }
            count++;
        }
    }
    closedir(dr);

    if (outCount)
        *outCount = count;
    return fileArray;
}

/** @brief Libera un arreglo de strings asignado dinamicamente. */
void freeStringArray(char **array, int n)
{
    for (int i = 0; i < n; i++)
    {
        if(array[i])
            free(array[i]);
    }
    if(array)
        free(array);
}

// ============================================================
// Debug
// ============================================================

/** @brief Imprime texto en stderr solo si el modo debug esta activado. */
void printDebug(logLevel level, char *error, ...)
{
    if (!config.debug_mode)
        return;

    static const char *log_level_str[] = {"INFO", "WARN", "ERROR"};
    
    va_list args;

    va_start(args, error);
    vfprintf(stderr, error, args);
    va_end(args);

    if (logFile)
    {
        fprintf(logFile, "[%s] [%-5s]", get_date(ALL, DASH, ISO), log_level_str[level]);
        va_start(args, error);
        vfprintf(logFile, error, args);
        va_end(args);
        fflush(logFile);
    }
}


/** @brief Devuelve un string legible del tipo de recurso admitido. */
char *typeAdmited(valid_type type)
{
    switch(type)
    {
        case IMAGE:
            return "imagen";
        case SOUND:
            return "sonido";
        default:
            return "desconocido";
    }
}

// ============================================================
// Utilidades SDL
// ============================================================

/** @brief Obtiene el ancho y alto de una textura SDL. */
void GetTextureSize(SDL_Texture* texture, int* width, int* height)
{
    SDL_QueryTexture(texture, NULL, NULL, width, height);
}

/** @brief Crea y devuelve un SDL_Color con los componentes indicados. */
SDL_Color setColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_Color color = {
        .r = r,
        .g = g,
        .b = b,
        .a = a
    };
    return color;
}

// ============================================================
// Utilidades de strings
// ============================================================

/** @brief Devuelve la longitud en bytes del string mas largo de un arreglo. */
int largestStr(char *arr[], int n)
{
    if(n <= 0)
    {
        perror("No array length");
        exit(1);
    }
    int max = 0, count = 0;
    for(int i = 0; i < n; i++)
    {
        count = (int)strlen(arr[i]);
        if(count > max)
            max = count;
    }
    return max;
}

/** @brief Devuelve la longitud en caracteres UTF-8 del string mas largo de un arreglo. */
int largestStr_bra(char *arr[], int n)
{
    if(n <= 0)
    {
        perror("No array length");
        exit(1);
    }
    int max = 0, count = 0;
    for(int i = 0; i < n; i++)
    {
        count = (int)u8_len(arr[i]);
        if(count > max)
            max = count;
    }
    return max;
}

/** @brief Calcula el ancho optimo para mostrar un menu de opciones con titulo. */
int largestOpt(char *choices[], int n_choice, const char *title)
{
    int length = largestStr(choices, n_choice);
    if(title && length < (int)strlen(title))
        return (int)strlen(title) + 4;
    else
        return length + 6;
}

/** @brief Cuenta la longitud de un string en caracteres multibyte (UTF-8). */
size_t u8_len(const char *s)
{
    mbstate_t st = {0};
    size_t count = 0;
    const char *p = s;
    wchar_t wc;
    while (*p)
    {
        size_t r = mbrtowc(&wc, p, MB_CUR_MAX, &st);
        if (r == (size_t)-1 || r == (size_t)-2)
            break;
        if (r == 0)
            break;
        count++;
        p += r;
    }
    return count;
}

/** @brief Reemplaza un elemento del arreglo aplicando su formato con un argumento. */
int replace_fmt(char **arr, int idx, const char *arg)
{
    if (!arr || idx < 0 || !arg)
        return -1;

    const char *tmpl = arr[idx];
    if (!tmpl)
        return -1;

    size_t needed = (size_t)snprintf(NULL, 0, tmpl, arg);
    char *buf = malloc(needed + 1);
    if (!buf)
        return -1;

    snprintf(buf, needed + 1, tmpl, arg);
    arr[idx] = buf;
    return 0;
}

// ============================================================
// Lectura de archivos de texto
// ============================================================

/** @brief Obtiene cantidad de lineas o ancho maximo de un archivo de texto. */
unsigned long fileLines(const char *file, int opt)
{
    FILE *txt = fopen(file, "r");
    if(!txt)
    {
        printf("Error: file '%s' not found\n", file);
        return 0;
    }
    unsigned long lines_count = 0, char_count = 0, WMAX = 0;
    int t;
    while ((t = fgetc(txt)) != EOF)
    {
        if (t == '\n' || t == '\r')
        {
            if(char_count > WMAX)
                WMAX = char_count;
            lines_count++;
            char_count = 0;
        }
        else
        {
            char_count++;
        }
    }
    if(lines_count > 1)
        lines_count++; //Linea adicional porque no existe \n inicial
    fclose(txt);
    if (opt == 0)
        return lines_count;
    else
        return WMAX;
}

/** @brief Lee un archivo de texto completo y devuelve un arreglo de strings. */
char **readText(const char *file)
{
    unsigned long lines = fileLines(file, 0);
    unsigned long chars = fileLines(file, 1);
    if (lines == 0 || chars == 0)
        return NULL;

    FILE *txt = fopen(file, "r");
    if (!txt)
        return NULL;

    char **arr = calloc(lines, sizeof(char *));
    if (!arr) {
        fclose(txt);
        return NULL;
    }

    for (unsigned long i = 0; i < lines; i++) {
        arr[i] = calloc(chars + 2, sizeof(char));
        if (!arr[i]) {
            fclose(txt);
            for (unsigned long j = 0; j < i; j++)
                free(arr[j]);
            free(arr);
            return NULL;
        }
        if (!fgets(arr[i], (int)(chars + 2), txt))
            arr[i][0] = '\0';
        arr[i][strcspn(arr[i], "\r\n")] = '\0';
    }
    fclose(txt);
    return arr;
}

int centerI(int a, int b)
{
    return abs((a - b)) / 2;
}

// ============================================================
// Metricas de sistema
// ============================================================

float getCpuUsage(void)
{
    static unsigned long prev_utime = 0, prev_stime = 0;
    static Uint32 prev_ticks = 0;

    FILE *f = fopen("/proc/self/stat", "r");
    if (!f)
        return -1.0f;

    char buf[512];
    if (!fgets(buf, sizeof(buf), f))
    {
        fclose(f);
        return -1.0f;
    }
    fclose(f);

    // El campo "comm" puede contener espacios, buscar el ultimo ')'
    char *p = strrchr(buf, ')');
    if (!p)
        return -1.0f;

    p += 2;

    // Saltar 11 campos (state..cmajflt) para llegar a utime(14) y stime(15)
    unsigned long dummy, utime, stime;
    char state;
    sscanf(p, "%c %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           &state, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
           &dummy, &dummy, &dummy, &dummy, &utime, &stime);

    Uint32 now = SDL_GetTicks();
    float cpu  = 0.0f;

    if (prev_ticks > 0)
    {
        unsigned long delta_cpu  = (utime + stime) - (prev_utime + prev_stime);
        float         delta_wall = (now - prev_ticks) / 1000.0f;
        long          ticks_sec  = sysconf(_SC_CLK_TCK);

        if (delta_wall > 0)
            cpu = ((float)delta_cpu / ticks_sec) / delta_wall * 100.0f;
    }

    prev_utime = utime;
    prev_stime = stime;
    prev_ticks = now;

    return cpu;
}

long getMemoryUsageMB(void)
{
    FILE *f = fopen("/proc/self/status", "r");
    if (!f)
        return -1;

    char line[128];
    while (fgets(line, sizeof(line), f))
    {
        long val;
        if (sscanf(line, "RssAnon: %ld", &val) == 1)
        {
            fclose(f);
            return val / 1024;
        }
    }

    fclose(f);
    return -1;
}

char *get_date(timeMesureUnit unit, dateSeparator separator, dateFormat region)
{
    static char date[64];
    char sep;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    
    if(separator != SLASH && separator != DASH)
    {
        printDebug(LOG_ERROR, "Error, no se especifico un separador de tiempo valido\n");
        return NULL;
    }

    sep = 45 + separator;

    if(region == ISO)
    {
        snprintf(date, sizeof(date), "%04d-%02d-%02d %02d:%02d:%02d",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
        return date;
    }
    if(region == ISO_DEBUG)
    {
        snprintf(date, sizeof(date), "%04d-%02d-%02d_%02d-%02d-%02d",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
        return date;
    }

    switch(unit)
    {
        case DATE:
            switch(region)
            {
                case EU:
                    snprintf(date, sizeof(date), "%02d%c%02d%c%04d",
                            tm.tm_mday, sep, tm.tm_mon + 1, sep, tm.tm_year + 1900);
                    break;
                case USA:
                    snprintf(date, sizeof(date), "%02d%c%02d%c%04d",
                            tm.tm_mon + 1, sep, tm.tm_mday, sep, tm.tm_year + 1900);
                    break;
                default:
                    printDebug(LOG_ERROR, "Error, no se especifico un formato regional del tiempo valida.\n");
                    return NULL;
                    break;
            }
            break;
        case HOURS:
            snprintf(date, sizeof(date), "%02d:%02d:%02d",
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
            break;
        case ALL:
            switch(region)
            {
                case EU:
                    snprintf(date, sizeof(date), "%02d%c%02d%c%04d %02d:%02d:%02d",
                            tm.tm_mday, sep, tm.tm_mon + 1, sep, tm.tm_year + 1900,
                            tm.tm_hour, tm.tm_min, tm.tm_sec);
                    break;
                case USA:
                    snprintf(date, sizeof(date), "%02d%c%02d%c%04d %02d:%02d:%02d",
                            tm.tm_mon + 1, sep, tm.tm_mday, sep, tm.tm_year + 1900,
                            tm.tm_hour, tm.tm_min, tm.tm_sec);
                    break;
                default:
                    printDebug(LOG_ERROR, "Error, no se especifico un formato regional del tiempo valida.\n");
                    return NULL;
                    break;
            }
            break;
        case YEAR:
            snprintf(date, sizeof(date), "%04d", tm.tm_year + 1900);
            break;
        case MONTH:
            snprintf(date, sizeof(date), "%02d", tm.tm_mon + 1);
            break;
        case DAY:
            snprintf(date, sizeof(date), "%02d", tm.tm_mday);
            break;
        case HOUR:
            snprintf(date, sizeof(date), "%02d", tm.tm_hour);
            break;
        case MINUTE:
            snprintf(date, sizeof(date), "%02d", tm.tm_min);
            break;
        case SECONDS:
            snprintf(date, sizeof(date), "%02d", tm.tm_sec);
            break;
        default:
            printDebug(LOG_ERROR, "Error, no se especifico una unidad de medida de tiempo valida.\n");
            return NULL;
            break;
    }
    return date;
}

void initLog()
{
    if(!DirExists(LOGS_DIR))
        mkdir(LOGS_DIR, 0755);

    char logFileName[128];
    snprintf(logFileName, sizeof(logFileName), "%slog_%s.log", LOGS_DIR, get_date(0, 0, ISO_DEBUG));
    logFile = fopen(logFileName, "a");
    if(!logFile)
        printDebug(LOG_ERROR, "Error, no se pudo crear el archivo log...\n");
}

void closeLog()
{
    if(logFile)
    {
        fclose(logFile);
        logFile = NULL;
    }
    else
        return;
}

#define TOOLS_DEBUG

#ifdef TOOLS_DEBUG

int main()
{
    config.debug_mode = true;
    initLog();
    printDebug(LOG_INFO, "Hola log file del %s!\n", get_date(ALL, DASH, ISO));
    closeLog();

    return 0;
}

#endif
