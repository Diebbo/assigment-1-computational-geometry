"""python script that generates an array of size -n <size> and writes on the file -o <output_file>
the length of the array as first line and the array as one element per line"""

import argparse
import random
import time


def generate_shuffle_array(size):
    """Generates an array of random integers of given size, no duplicates"""
    array = set()
    while len(array) < size:
        array.add(random.randrange(0, 2**31))
    array = list(array)
    random.shuffle(array)
    return array


def write_array_to_file(array, output_file):
    """Writes the array to the output file, first line is the length of the array"""
    with open(output_file, "w") as f:
        f.write(f"{len(array)}\n")
        for item in array:
            f.write(f"{item}\n")


def main():
    """Main function to parse arguments and generate the array."""
    parser = argparse.ArgumentParser(
        description="Generate an array of random integers and write to a file."
    )
    parser.add_argument(
        "-n", "--size", type=int, required=True, help="Size of the array to generate."
    )
    parser.add_argument(
        "-s", "--sorted", help="Output a sorted array.", action=argparse.BooleanOptionalAction
    )
    parser.add_argument(
        "-o",
        "--output_file",
        type=str,
        required=True,
        help="Output file to write the array.",
    )
    args = parser.parse_args()

    size = args.size
    output_file = args.output_file

    if size <= 0:
        print("Size must be a positive integer.")
        return

    # get the absolute path of the output file
    output_file = output_file if output_file.startswith("/") else f"./{output_file}"

    start_time = time.time()
    array = generate_shuffle_array(size)
    if args.sorted:
        array.sort()
    write_array_to_file(array, output_file)
    end_time = time.time()

    print(
        f"Array of size {size} written to {output_file} in {end_time - start_time:.4f} seconds."
    )


if __name__ == "__main__":
    main()
