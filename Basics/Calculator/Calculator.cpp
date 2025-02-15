/**
 * @file calculator.cpp
 * @brief A calculator that evaluates mathematical expressions, and exponentiation.
 *
 * This program implements a recursive descent parser to evaluate mathematical expressions based on standard mathematical
 * operator precedence. It supports the following operations:
 *   - Addition (+)
 *   - Subtraction (-)
 *   - Multiplication (*)
 *   - Division (/)
 *   - Exponentiation (^)
 *   - Parentheses for grouping (...)
 *
 * How the program works:
 * 1. The user is prompted to enter a mathematical expression.
 * 2. The expression is parsed using a recursive descent parser that breaks the expression into tokens and recursively
 *    evaluates it according to operator precedence.
 * 3. Exponentiation is handled as a right-associative operator.
 * 4. The result of the evaluation is printed.
 * 5. The program continues accepting expressions until the user types "exit".
 *
 * @note This file is intended for educational purposes and demonstrates how to build a calculator in C++.
 *
 * @author teskny
 * @date 16:40 on Frebruary 15, 2025
 */

 #include <iostream>
 #include <sstream>
 #include <stdexcept>
 #include <string>
 #include <cctype>
 #include <cmath>
 
 /**
  * @brief A class that implements a recursive descent parser to evaluate arithmetic expressions.
  *
  * This Parser class reads an expression as a string and interprets it using a set of recursive functions.
  * Each function handles a level of the grammar:
  * - parseExpression(): Addition and Subtraction are handled here.
  * - parseTerm(): Multiplication and Division are handled here.
  * - parseExponent(): Exponentiation (^) is handled here recursively.
  * - parsePrimary(): Handles numbers, parentheses, and unary operations (+ and -).
  *
  * The parser checks error and can throw exceptions for invalid syntax.
  */
 class Parser {
 public:
     /**
      * @brief Constructs a new Parser object.
      * @param expression The arithmetic expression to be parsed as a std::string.
      */
     explicit Parser(const std::string &expression)
         : expr(expression), pos(0) {}
 
     /**
      * @brief Evaluates the arithmetic expression provided to the parser.
      * @return double The result of the evaluated expression.
      * @throws std::runtime_error if a syntax error or invalid token is encountered.
      *
      * This function is the entry point for parsing. It calls parseExpression() to evaluate the entire
      * expression and then checks if the complete input string has been processed. If not, it indicates an error.
      */
     double parse() {
         double result = parseExpression();
         skipWhitespace();
         if (pos != expr.length()) {
             throw std::runtime_error("Unexpected token at position " + std::to_string(pos) + ": '" + currentChar() + "'");
         }
         return result;
     }
 
 private:
     std::string expr; ///< The input expression as a string.
     std::size_t pos;  ///< The current position in the string during parsing.
 
     /**
      * @brief Skips whitespace characters in the expression.
      *
      * Advances the parsing position past any spaces or tab characters.
      */
     void skipWhitespace() {
         while (pos < expr.size() && std::isspace(expr[pos])) {
             ++pos;
         }
     }
 
     /**
      * @brief Returns the current character that the parser is looking at.
      * @return char The current character, or '\0' if the end of the string is reached.
      */
     char currentChar() const {
         return pos < expr.length() ? expr[pos] : '\0';
     }
 
     /**
      * @brief Parses an expression with addition and subtraction.
      * @return double The computed result for the expression.
      *
      * This function recognizes the '+' and '-' operators. It first evaluates a term, then iteratively looks for
      * '+' or '-' signs to combine additional terms.
      */
     double parseExpression() {
         double value = parseTerm();
         while (true) {
             skipWhitespace();
             if (match('+')) {
                 double rhs = parseTerm();
                 value += rhs;
             } else if (match('-')) {
                 double rhs = parseTerm();
                 value -= rhs;
             } else {
                 break;
             }
         }
         return value;
     }
 
     /**
      * @brief Parses a term with multiplication and division.
      * @return double The computed result for the term.
      *
      * This function recognizes the '*' and '/' operators. It first evaluates an exponent expression, then iteratively
      * combines subsequent exponent expressions with multiplication or division.
      */
     double parseTerm() {
         double value = parseExponent();
         while (true) {
             skipWhitespace();
             if (match('*')) {
                 double rhs = parseExponent();
                 value *= rhs;
             } else if (match('/')) {
                 double rhs = parseExponent();
                 if (rhs == 0.0) {
                     throw std::runtime_error("Division by zero at position " + std::to_string(pos));
                 }
                 value /= rhs;
             } else {
                 break;
             }
         }
         return value;
     }
 
     /**
      * @brief Parses exponentiation expressions, supporting the '^' operator.
      * @return double The computed result for exponentiation.
      *
      * Exponentiation is treated as right-associative. This means that an expression like "a ^ b ^ c" is interpreted as
      * "a ^ (b ^ c)". This function first parses a primary level expression and then, if a '^' is found, recursively
      * parses the exponentiation on the right side.
      */
     double parseExponent() {
         double base = parsePrimary();
         skipWhitespace();
         if (match('^')) {
             // Exponentiation is right-associative: parse the exponent recursively.
             double exponent = parseExponent();
             // Use std::pow from <cmath> to calculate the result.
             return std::pow(base, exponent);
         }
         return base;
     }
 
     /**
      * @brief Parses primary expressions like numbers, parenthesized expressions, and unary operations.
      * @return double The computed result for the primary expression.
      *
      * This function handles three cases:
      * - Unary plus or minus operators: e.g., "-5" or "+3".
      * - Parenthesized expressions: e.g., "(2 + 3)".
      * - Numeric values: reads a number from the input string.
      */
     double parsePrimary() {
         skipWhitespace();
         // Handle unary operators.
         if (match('+')) {
             return parsePrimary();
         } else if (match('-')) {
             return -parsePrimary();
         }
 
         double value = 0.0;
         if (match('(')) {
             // If an open parenthesis is found, parse the expression inside.
             value = parseExpression();
             skipWhitespace();
             if (!match(')')) {
                 throw std::runtime_error("Missing closing parenthesis at position " + std::to_string(pos));
             }
         } else {
             // Otherwise, the token must represent a number.
             value = parseNumber();
         }
         return value;
     }
 
     /**
      * @brief Parses a numerical value from the expression.
      * @return double The parsed number.
      * @throws std::runtime_error if no valid number is found.
      *
      * This function reads consecutive digits and at most one decimal point to form a valid floating point number.
      * It uses the standard library function std::stod to convert the substring to a double.
      */
     double parseNumber() {
         skipWhitespace();
         std::size_t start = pos;
         bool decimalPointSeen = false;
         // Process each character that might belong to a number (digit or decimal point).
         while (pos < expr.size() && (std::isdigit(expr[pos]) || expr[pos] == '.')) {
             if (expr[pos] == '.') {
                 if (decimalPointSeen) {
                     throw std::runtime_error("Invalid number format at position " + std::to_string(pos));
                 }
                 decimalPointSeen = true;
             }
             ++pos;
         }
         if (start == pos) {
             throw std::runtime_error("Expected a number at position " + std::to_string(pos));
         }
         double number = 0.0;
         try {
             number = std::stod(expr.substr(start, pos - start));
         } catch (const std::exception &e) {
             throw std::runtime_error("Number conversion error at position " + std::to_string(start));
         }
         return number;
     }
 
     /**
      * @brief Checks if the current character matches the expected token.
      * @param expected The character expected at the current position.
      * @return true If the current character matches the expected one (and advances the position).
      * @return false Otherwise.
      *
      * This helper function is used to validate the current token in the expression.
      */
     bool match(char expected) {
         skipWhitespace();
         if (pos < expr.size() && expr[pos] == expected) {
             ++pos;
             return true;
         }
         return false;
     }
 };
 
 /**
  * @brief The main entry point for the calculator program.
  *
  * This function drives the calculator by repeatedly prompting the user for input. It:
  *   1. Displays a welcome message.
  *   2. Reads a line of input from the user.
  *   3. If the user types "exit", the program terminates.
  *   4. Otherwise, it creates an instance of Parser to evaluate the input expression.
  *   5. Prints the result or an error message if the expression is invalid.
  *
  * Explanation:
  * - The std::string class is used to store text.
  * - std::getline is used to read a full line of input from the standard input (keyboard).
  * - A try-catch block is used to handle exceptions (errors). If the Parser throws an exception because the expression
  *   is invalid, the catch block prints an error message.
  * - The program continues to loop until the user types "exit".
  *
  * @return int Returns 0 if the program completes successfully.
  */
 int main() {
     std::cout << "C++ Calculator (type 'exit' to quit)" << std::endl;
     std::string input;
 
     // Continuously prompt the user for expressions to evaluate.
     while (true) {
         std::cout << "\nEnter expression: ";
         std::getline(std::cin, input);
 
         // Check if the user wants to exit the program.
         if (input == "exit") {
             std::cout << "Exiting calculator. Goodbye!" << std::endl;
             break;
         }
         try {
             // Create a Parser object with the user's input and evaluate the expression.
             Parser parser(input);
             double result = parser.parse();
             std::cout << "Result: " << result << std::endl;
         } catch (const std::exception &ex) {
             // If an error occurs during parsing or evaluation, display the error message.
             std::cerr << "Error: " << ex.what() << std::endl;
         }
     }
     return 0;
 };
