#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_MOVIES 100
#define FILE_NAME "movies.txt"

struct Movie {
    int id;
    char name[100];
    char genre[50];
    int year;
    int watched;
};

struct Movie movies[MAX_MOVIES];
int movieCount = 0;
int nextMovieId = 101;

// Function declarations
void loadMovies();
void saveMovies();
void addMovie();
void displayMovies();
void searchMovie();
void updateMovie();
void deleteMovie();
void markWatched();

int readInteger(const char prompt[]);
void readString(const char prompt[], char str[], int size);
void clearInputBuffer();
int findMovieById(int id);
int containsIgnoreCase(const char text[], const char search[]);
int getMatchingMovies(const char search[]);
int selectMovieByName(const char prompt[]);

// Main function
int main() {
    int choice;

    loadMovies();

    printf("\n====================================\n");
    printf("       MOVIE MANAGEMENT SYSTEM\n");
    printf("====================================\n");

    do {
        printf("\n----------- MAIN MENU -----------\n");
        printf("1. Add Movie\n");
        printf("2. Display All Movies\n");
        printf("3. Search Movie\n");
        printf("4. Update Movie\n");
        printf("5. Delete Movie\n");
        printf("6. Mark Watched / Unwatched\n");
        printf("7. Exit\n");
        printf("---------------------------------\n");

        choice = readInteger("Enter your choice: ");

        switch (choice) {
            case 1:
                addMovie();
                break;

            case 2:
                displayMovies();
                break;

            case 3:
                searchMovie();
                break;

            case 4:
                updateMovie();
                break;

            case 5:
                deleteMovie();
                break;

            case 6:
                markWatched();
                break;

            case 7:
                saveMovies();
                printf("\nThank you for using Movie Management System!\n");
                break;

            default:
                printf("\nInvalid choice! Please try again.\n");
        }

    } while (choice != 7);

    return 0;
}

// Clear unwanted input from buffer
void clearInputBuffer() {
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF);
}

// Read an integer safely
int readInteger(const char prompt[]) {
    char input[100];
    char *end;
    long value;

    while (1) {
        printf("%s", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Input error!\n");
            exit(1);
        }

        value = strtol(input, &end, 10);

        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (end != input && *end == '\0' &&
            value >= 0 && value <= 2147483647L) {
            return (int)value;
        }

        printf("Invalid input! Enter a valid number.\n");
    }
}

// Read a string including spaces
void readString(const char prompt[], char str[], int size) {
    size_t length;

    while (1) {
        printf("%s", prompt);

        if (fgets(str, size, stdin) == NULL) {
            printf("Input error!\n");
            exit(1);
        }

        length = strlen(str);

        if (length > 0 && str[length - 1] == '\n') {
            str[length - 1] = '\0';
        } else {
            clearInputBuffer();
        }

        if (str[0] != '\0') {
            return;
        }

        printf("Input cannot be empty!\n");
    }
}

// Find movie using ID
int findMovieById(int id) {
    for (int i = 0; i < movieCount; i++) {
        if (movies[i].id == id) {
            return i;
        }
    }

    return -1;
}

// Check partial movie name without case sensitivity
int containsIgnoreCase(const char text[], const char search[]) {
    int textLength = (int)strlen(text);
    int searchLength = (int)strlen(search);

    if (searchLength == 0 || searchLength > textLength) {
        return 0;
    }

    for (int i = 0; i <= textLength - searchLength; i++) {
        int j = 0;

        while (j < searchLength &&
               tolower((unsigned char)text[i + j]) ==
               tolower((unsigned char)search[j])) {
            j++;
        }

        if (j == searchLength) {
            return 1;
        }
    }

    return 0;
}

// Display all movies matching a name
int getMatchingMovies(const char search[]) {
    int matches = 0;

    for (int i = 0; i < movieCount; i++) {
        if (containsIgnoreCase(movies[i].name, search)) {
            if (matches == 0) {
                printf("\nMatching Movies:\n");
                printf("%-8s %-25s %-15s %-8s %-12s\n",
                       "ID", "NAME", "GENRE", "YEAR", "STATUS");
                printf("---------------------------------------------------------------------\n");
            }

            printf("%-8d %-25s %-15s %-8d %-12s\n",
                   movies[i].id,
                   movies[i].name,
                   movies[i].genre,
                   movies[i].year,
                   movies[i].watched ? "Watched" : "Unwatched");

            matches++;
        }
    }

    return matches;
}

// Select a movie by searching its name
int selectMovieByName(const char prompt[]) {
    char search[100];
    int matches;
    int selected;
    int choice;
    int count = 0;
    int indices[MAX_MOVIES];

    if (movieCount == 0) {
        printf("\nNo movies available!\n");
        return -1;
    }

    readString(prompt, search, sizeof(search));

    printf("\n");
    
    for (int i = 0; i < movieCount; i++) {
        if (containsIgnoreCase(movies[i].name, search)) {
            indices[count] = i;
            count++;
        }
    }

    matches = count;

    if (matches == 0) {
        printf("Movie not found!\n");
        return -1;
    }

    if (matches == 1) {
        return indices[0];
    }

    printf("Multiple movies found. Select one:\n");

    for (int i = 0; i < matches; i++) {
        int index = indices[i];

        printf("%d. %s (ID: %d)\n",
               i + 1, movies[index].name, movies[index].id);
    }

    choice = readInteger("Enter your selection: ");

    if (choice < 1 || choice > matches) {
        printf("Invalid selection!\n");
        return -1;
    }

    selected = indices[choice - 1];

    return selected;
}

// Load movies from file
void loadMovies() {
    FILE *fp = fopen(FILE_NAME, "r");
    int maxId = 100;

    if (fp == NULL) {
        return;
    }

    movieCount = 0;

    while (movieCount < MAX_MOVIES &&
           fscanf(fp, "%d\n%99[^\n]\n%49[^\n]\n%d\n%d\n",
                  &movies[movieCount].id,
                  movies[movieCount].name,
                  movies[movieCount].genre,
                  &movies[movieCount].year,
                  &movies[movieCount].watched) == 5) {

        if (movies[movieCount].id > maxId) {
            maxId = movies[movieCount].id;
        }

        movieCount++;
    }

    fclose(fp);

    nextMovieId = maxId + 1;
}

// Save movies to file
void saveMovies() {
    FILE *fp = fopen(FILE_NAME, "w");

    if (fp == NULL) {
        printf("\nError saving movie records!\n");
        return;
    }

    for (int i = 0; i < movieCount; i++) {
        fprintf(fp, "%d\n%s\n%s\n%d\n%d\n",
                movies[i].id,
                movies[i].name,
                movies[i].genre,
                movies[i].year,
                movies[i].watched);
    }

    fclose(fp);
}

// Add a new movie
void addMovie() {
    struct Movie m;

    if (movieCount >= MAX_MOVIES) {
        printf("\nMovie storage is full!\n");
        return;
    }

    if (nextMovieId <= 0) {
        printf("\nUnable to generate a new Movie ID!\n");
        return;
    }

    printf("\n---------- ADD MOVIE ----------\n");

    m.id = nextMovieId++;

    readString("Enter Movie Name: ", m.name, sizeof(m.name));
    readString("Enter Genre: ", m.genre, sizeof(m.genre));

    m.year = readInteger("Enter Release Year: ");

    if (m.year <= 0) {
        printf("Invalid release year!\n");
        nextMovieId--;
        return;
    }

    m.watched = 0;

    movies[movieCount] = m;
    movieCount++;

    saveMovies();

    printf("\nMovie added successfully!\n");
    printf("Automatically generated Movie ID: %d\n", m.id);
}

// Display all movies
void displayMovies() {
    printf("\n---------- MOVIE LIST ----------\n");

    if (movieCount == 0) {
        printf("No movies available.\n");
        return;
    }

    printf("%-8s %-25s %-15s %-8s %-12s\n",
           "ID", "NAME", "GENRE", "YEAR", "STATUS");

    printf("---------------------------------------------------------------------\n");

    for (int i = 0; i < movieCount; i++) {
        printf("%-8d %-25s %-15s %-8d %-12s\n",
               movies[i].id,
               movies[i].name,
               movies[i].genre,
               movies[i].year,
               movies[i].watched ? "Watched" : "Unwatched");
    }
}

// Search movie by name
void searchMovie() {
    char search[100];

    printf("\n---------- SEARCH MOVIE ----------\n");

    if (movieCount == 0) {
        printf("No movies available!\n");
        return;
    }

    readString("Enter Movie Name to search: ",
               search, sizeof(search));

    int matches = getMatchingMovies(search);

    if (matches == 0) {
        printf("Movie not found!\n");
    } else {
        printf("\nTotal matching movies: %d\n", matches);
    }
}

// Update movie details
void updateMovie() {
    int index;
    int year;

    printf("\n---------- UPDATE MOVIE ----------\n");

    index = selectMovieByName("Enter Movie Name to update: ");

    if (index == -1) {
        return;
    }

    printf("\nSelected Movie: %s\n", movies[index].name);

    readString("Enter New Movie Name: ",
               movies[index].name,
               sizeof(movies[index].name));

    readString("Enter New Genre: ",
               movies[index].genre,
               sizeof(movies[index].genre));

    year = readInteger("Enter New Release Year: ");

    if (year <= 0) {
        printf("Invalid release year! Update cancelled.\n");
        return;
    }

    movies[index].year = year;

    saveMovies();

    printf("\nMovie updated successfully!\n");
}

// Delete movie
void deleteMovie() {
    int index;

    printf("\n---------- DELETE MOVIE ----------\n");

    index = selectMovieByName("Enter Movie Name to delete: ");

    if (index == -1) {
        return;
    }

    printf("Selected Movie: %s\n", movies[index].name);

    for (int i = index; i < movieCount - 1; i++) {
        movies[i] = movies[i + 1];
    }

    movieCount--;

    saveMovies();

    printf("\nMovie deleted successfully!\n");
}

// Mark movie as watched or unwatched
void markWatched() {
    int index;
    int status;

    printf("\n---------- UPDATE WATCH STATUS ----------\n");

    index = selectMovieByName("Enter Movie Name: ");

    if (index == -1) {
        return;
    }

    printf("\nSelected Movie: %s\n", movies[index].name);
    printf("Current Status: %s\n",
           movies[index].watched ? "Watched" : "Unwatched");

    printf("\n1. Watched\n");
    printf("0. Unwatched\n");

    status = readInteger("Enter new status: ");

    if (status != 0 && status != 1) {
        printf("Invalid status!\n");
        return;
    }

    movies[index].watched = status;

    saveMovies();

    printf("\nWatch status updated successfully!\n");
}

