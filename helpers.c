#include "helpers.h"
#include <math.h>
#include <stdlib.h>

// Cap a colour value at 255 so it fits in a BYTE
static int cap(int value)
{
    return value > 255 ? 255 : value;
}

// Convert image to greyscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Divide by 3.0 (a float) so the average is not truncated as integer division
            int avg = round((image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue) / 3.0);

            // Same value in all three channels gives a shade of grey
            image[i][j].rgbtRed = avg;
            image[i][j].rgbtGreen = avg;
            image[i][j].rgbtBlue = avg;
        }
    }
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Save the original values: all three formulas must use the ORIGINAL pixel
            int r = image[i][j].rgbtRed;
            int g = image[i][j].rgbtGreen;
            int b = image[i][j].rgbtBlue;

            // Apply the sepia formulas, round each result, cap at 255
            int sepiaRed = cap(round(.393 * r + .769 * g + .189 * b));
            int sepiaGreen = cap(round(.349 * r + .686 * g + .168 * b));
            int sepiaBlue = cap(round(.272 * r + .534 * g + .131 * b));

            image[i][j].rgbtRed = sepiaRed;
            image[i][j].rgbtGreen = sepiaGreen;
            image[i][j].rgbtBlue = sepiaBlue;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        // Only go to the halfway point: going further would swap everything back
        for (int j = 0; j < width / 2; j++)
        {
            RGBTRIPLE tmp = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = tmp;
        }
    }
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // THE COPY PROBLEM: make a copy of the untouched image first.
    // All averages are read from the copy and written to the original,
    // so a pixel is never averaged with an already-blurred neighbour.
    RGBTRIPLE(*copy)[width] = malloc(height * sizeof(*copy));
    if (copy == NULL)
    {
        return;
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            copy[i][j] = image[i][j];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumRed = 0, sumGreen = 0, sumBlue = 0;
            int count = 0;

            // Visit the 3x3 box centred on (i, j): rows i-1..i+1, columns j-1..j+1
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;

                    // Skip neighbours that fall outside the image (edges and corners)
                    if (ni < 0 || ni >= height || nj < 0 || nj >= width)
                    {
                        continue;
                    }

                    // Read from the COPY, never from the image being changed
                    sumRed += copy[ni][nj].rgbtRed;
                    sumGreen += copy[ni][nj].rgbtGreen;
                    sumBlue += copy[ni][nj].rgbtBlue;
                    count++;
                }
            }

            // Divide as a float, then round, using only the neighbours that existed
            image[i][j].rgbtRed = round((float) sumRed / count);
            image[i][j].rgbtGreen = round((float) sumGreen / count);
            image[i][j].rgbtBlue = round((float) sumBlue / count);
        }
    }

    free(copy);
}
