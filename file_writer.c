#include <stdio.h>

int main() {
  FILE *fpt;
  fpt = fopen("./res/index.html", "w");
  char body[] = "<!DOCTYPE html>\n"
                "<html lang=\"en\">\n"
                "<head>\n"
                "    <meta charset=\"UTF-8\">\n"
                "    <meta name=\"viewport\" content=\"width=device-width, "
                "initial-scale=1.0\">\n"
                "    <title>Doc</title>\n"
                "    <link rel=\"stylesheet\" href=\"style.css\">\n"
                "</head>\n"
                "<body>\n"
                "    <h1>Hello, World!</h1>\n"
                "    <script src=\"script.js\"></script>\n"
                "</body>\n"
                "</html>";
  fprintf(fpt, "%s", body);
  return 0;
}
