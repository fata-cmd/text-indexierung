import os
import random
import string

def generate_random_word(length):
    """Generate a random word of a given length."""
    return ''.join(random.choices(string.ascii_lowercase, k=length))

def create_large_text_file(filename, target_size_gb):
    """Create a text file with random words until it reaches the target size in GB."""
    target_size_bytes = target_size_gb * (1024 ** 3)  # Convert GB to bytes
    
    with open(filename, 'w') as f:
        current_size = 0
        while current_size < target_size_bytes:
            length = random.randint(3, 8)
            word = generate_random_word(length)
            f.write(word + '\n')
            current_size += length + 1

if __name__ == "__main__":
    create_large_text_file("random_words.txt", 0.4)
