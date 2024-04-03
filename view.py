import numpy as np
import matplotlib.pyplot as plt

# Function to read image dimensions and data from a custom binary format
def read_custom_bin_image(filename):
    with open(filename, 'rb') as f:
        # Read the first 4 bytes: height and width as 16-bit unsigned integers
        height = int.from_bytes(f.read(2), 'big')
        width = int.from_bytes(f.read(2), 'big')
        
        # Assuming the rest of the file is RGB data
        data = np.fromfile(f, dtype=np.uint8)
        image = data.reshape((height, width, 3))
        
    return image

# Reading the image and its dimensions
image = read_custom_bin_image('HJoceanSmall_Carved.bin')

# Displaying the image
plt.imshow(image)
plt.axis('off')  # Hide axes for a cleaner look
plt.show()
