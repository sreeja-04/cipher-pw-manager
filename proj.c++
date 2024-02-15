#include <stdio.h>
#include <string.h>
#include <math.h>
#include <openssl/rand.h>

#define MAX_KEY_SIZE 4 // Maximum size of the encryption key matrix
#define MAX_TEXT_SIZE 1000 // Maximum size of the input text
#define ALPHABET_SIZE 26

void generateRandomKey(int key[MAX_KEY_SIZE][MAX_KEY_SIZE]);
int isInvertibleKey(const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]);
int determinant(const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]);
void inverseKey(int invKey[MAX_KEY_SIZE][MAX_KEY_SIZE], const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]);
void multiplyMatrix(int result[MAX_KEY_SIZE][MAX_KEY_SIZE], const int matrix1[MAX_KEY_SIZE][MAX_KEY_SIZE], const int matrix2[MAX_KEY_SIZE][MAX_KEY_SIZE]);
void encryptHill(const char* inputFile, const char* outputFile, const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]);
void decryptHill(const char* inputFile, const char* outputFile, const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]);

int main() 
{
    int key[MAX_KEY_SIZE][MAX_KEY_SIZE];

    // Generate a random key matrix
    generateRandomKey(key);

    char inputFile[20];
    char outputFile[20] = "a";
    char choice;

    printf("Enter the name of the file: ");
    scanf("%s", inputFile);

    FILE* inputFilePtr = fopen(inputFile, "r");
    
    if(inputFilePtr == NULL) 
    {
        printf("Error: The given file does not exist in the scope!\n");
        return 0;
    } 
    else 
    {
        fclose(inputFilePtr);
        strcat(outputFile, inputFile);

        FILE* outputFilePtr = fopen(outputFile, "w");
        fclose(outputFilePtr);
    }

    printf("Do you want to encrypt or decrypt the file?(E/D): ");
    scanf("\n%c", &choice);

    if(choice == 'E' || choice == 'e') 
    {
        encryptHill(inputFile, outputFile, key);
    } 
    else if(choice == 'D' || choice == 'd') 
    {
        decryptHill(inputFile, outputFile, key);
    } 
    else 
    {
        printf("Invalid choice.\n");
    }

    return 0;
}

void generateRandomKey(int key[MAX_KEY_SIZE][MAX_KEY_SIZE]) 
{
    RAND_bytes((unsigned char*)key, MAX_KEY_SIZE * MAX_KEY_SIZE * sizeof(int));

    // Normalize the key matrix
    int sum = 0;
    for (int i = 0; i < MAX_KEY_SIZE; i++) 
    {
        for (int j = 0; j < MAX_KEY_SIZE; j++) 
        {
            sum += key[i][j];
        }
    }

    for (int i = 0; i < MAX_KEY_SIZE; i++) 
    {
        for (int j = 0; j < MAX_KEY_SIZE; j++) 
        {
            key[i][j] = (int)round((double)key[i][j] / sum * ALPHABET_SIZE);
        }
    }
}

int isInvertibleKey(const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]) 
{ 
    return determinant(key) != 0; 
}

int determinant(const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]) 
{
    int subDeterminants[MAX_KEY_SIZE];
    int sum = 0; 
    
    for (int i = 0; i < MAX_KEY_SIZE; i++) 
    { 
        int subMatrix[MAX_KEY_SIZE - 1][MAX_KEY_SIZE - 1]; 
        for (int j = 1; j < MAX_KEY_SIZE; j++) 
        { 
            int rowIndex = 0; 
            for (int k = 0; k < MAX_KEY_SIZE; k++) 
            { 
                if (k != i) 
                { 
                    subMatrix[j - 1][rowIndex++] = key[j][k]; 
                } 
            } 
        } 
        subDeterminants[i] = (i % 2 == 0) ? 1 : -1; 
        for (int j = 0; j < MAX_KEY_SIZE - 1; j++) 
        { 
            subDeterminants[i] *= subMatrix[j][j]; 
        } 
    } 
    return subDeterminants[0] + subDeterminants[1] + subDeterminants[2]; 
}

void inverseKey(int invKey[MAX_KEY_SIZE][MAX_KEY_SIZE], const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]) 
{
    int det = determinant(key);

    if (det == 0) 
    { 
        printf("Error: The key matrix is not invertible.\n"); 
        return; 
    }

    int augmentedMatrix[MAX_KEY_SIZE][2 * MAX_KEY_SIZE];
    for (int i = 0; i < MAX_KEY_SIZE; i++) 
    { 
        for (int j = 0; j < MAX_KEY_SIZE; j++) 
        { 
            augmentedMatrix[i][j] = key[i][j]; 
        } 
        augmentedMatrix[i][j + MAX_KEY_SIZE] = (i == j) ? 1 : 0; 
    }

    for (int row = 0; row < MAX_KEY_SIZE; row++) 
    { 
        if (augmentedMatrix[row][row] == 0) 
        { 
            for (int i = row + 1; i < MAX_KEY_SIZE; i++) 
            { 
                if (augmentedMatrix[i][row] != 0) 
                { 
                    for (int j = 0; j < 2 * MAX_KEY_SIZE; j++) 
                    { 
                        augmentedMatrix[row][j] = augmentedMatrix[i][j]; 
                    } 
                    break; 
                } 
            } 
        }

        for (int col = 0; col < 2 * MAX_KEY_SIZE; col++) 
        { 
            int div = augmentedMatrix[row][row]; 
            for (int j = row; j < 2 * MAX_KEY_SIZE; j++) 
            { 
                augmentedMatrix[row][j] /= div; 
            } 
            for (int i = 0; i < MAX_KEY_SIZE; i++) 
            { 
                if (i != row) 
                { 
                    int mult = augmentedMatrix[i][row]; 
                    for (int j = row; j < 2 * MAX_KEY_SIZE; j++) 
                    { 
                        augmentedMatrix[i][j] -= mult * augmentedMatrix[row][j]; 
                    } 
                } 
            } 
        } 
    }

    for (int i = 0; i < MAX_KEY_SIZE; i++) 
    { 
        for (int j = 0; j < MAX_KEY_SIZE; j++) 
        { 
            invKey[i][j] = augmentedMatrix[i][j + MAX_KEY_SIZE]; 
        } 
    } 
}

void multiplyMatrix(int result[MAX_KEY_SIZE][MAX_KEY_SIZE], const int matrix1[MAX_KEY_SIZE][MAX_KEY_SIZE], const int matrix2[MAX_KEY_SIZE][MAX_KEY_SIZE]) 
{
    for (int i = 0; i < MAX_KEY_SIZE; i++) 
    { 
        for (int j = 0; j < MAX_KEY_SIZE; j++) 
        { 
            result[i][j] = 0; 
            for (int k = 0; k < MAX_KEY_SIZE; k++) 
            { 
                result[i][j] += matrix1[i][k] * matrix2[k][j]; 
            } 
        } 
    } 
}

void encryptHill(const char* inputFile, const char* outputFile, const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]) {
    if (isInvertibleKey(key) == 0) { 
        printf("Error: The key matrix is not invertible.\n"); 
        return; 
    }

    FILE* inputFilePtr = fopen(inputFile, "r"); 
    FILE* outputFilePtr = fopen(outputFile, "w");

    char ch; 
    int text[MAX_TEXT_SIZE] = {0}; // Array to store numerical representation of characters 
    int encrypted[MAX_TEXT_SIZE] = {0}; // Array to store encrypted characters

    int count = 0; 

    while ((ch = fgetc(inputFilePtr)) != EOF && count < MAX_TEXT_SIZE) { 
        text[count++] = ch; 
    }

    int n = (int)sqrt((float)count); // Length of input text 

    if (n * n != count) { 
        n++; 
    } 
    int blockSize = n;

    for (int j = 0; j < count; j += blockSize) 
    { 
        for (int k = 0; k < blockSize; k++) 
        { 
            int sum = 0; 
            for (int l = 0; l < blockSize; l++) 
            { 
                sum += key[k][l] * text[j + l]; 
            } 
            encrypted[j + k] = sum % ALPHABET_SIZE; 
        } 
    }

    for (int j = 0; j < count; j++) { 
        fputc(encrypted[j] + 'a', outputFilePtr); 
    }

    fclose(inputFilePtr); 
    fclose(outputFilePtr); 

    printf("Encryption successful.\n"); 
}

void decryptHill(const char* inputFile, const char* outputFile, const int key[MAX_KEY_SIZE][MAX_KEY_SIZE]) 
{ 
    int invKey[MAX_KEY_SIZE][MAX_KEY_SIZE]; 
    inverseKey(invKey, key);

    FILE* inputFilePtr = fopen(inputFile, "r"); 
    FILE* outputFilePtr = fopen(outputFile, "w");

    char ch; 
    int text[MAX_TEXT_SIZE] = {0}; 
    int decrypted[MAX_TEXT_SIZE] = {0};

    int count = 0; 
    while ((ch = fgetc(inputFilePtr)) != EOF && count < MAX_TEXT_SIZE) 
    { 
        text[count++] = ch; 
    }

    int n = (int)sqrt((float)count); 
    if (n * n != count) 
    { 
        n++; 
    } 
    int blockSize = n;

    for (int j = 0; j < count; j += blockSize) 
    { 
        for (int k = 0; k < blockSize; k++) 
        { 
            int sum = 0; 
            for (int l = 0; l < blockSize; l++) 
            { 
                sum += invKey[k][l] * text[j + l]; 
            } 
            decrypted[j + k] = sum % ALPHABET_SIZE; 
        } 
    }

    for (int j = 0; j < count; j++) 
    { 
        fputc(decrypted[j] + 'a', outputFilePtr); 
    }

    fclose(inputFilePtr); 
    fclose(outputFilePtr); 
    printf("Decryption successful.\n"); 
}
