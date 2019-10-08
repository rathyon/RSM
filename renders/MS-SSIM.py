#usage:
# python -W ignore MS-SSIM.py path1 path2

import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import sys
import tensorflow as tf
tf.compat.v1.logging.set_verbosity(tf.compat.v1.logging.ERROR)


if len(sys.argv) != 3:
	print("Invalid argument count: file + path_to_img1 + path_to_img2")
	print(str(sys.argv))
	sys.exit(0)

path_to_img1 = sys.argv[1]
path_to_img2 = sys.argv[2]

im1_content = tf.read_file(path_to_img1)
im2_content = tf.read_file(path_to_img2)

im1 = tf.image.decode_png(im1_content, channels=3)
im2 = tf.image.decode_jpeg(im2_content, channels=3)

msssim = tf.image.ssim_multiscale(im1, im2, max_val=255, filter_size=11,
                          filter_sigma=1.5, k1=0.01, k2=0.03)

session = tf.compat.v1.Session()

result = session.run(msssim)

print(result)