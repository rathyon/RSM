import tensorflow as tf 

im1_content = tf.read_file(path_to_im1)
im2_content = tf.read_file(path_to_im2)

im1 = tf.image.decode_png(im1_content, channels=3)
im2 = tf.image.decode_jpeg(im2_content, channels=3)

ssim = tf.image.ssim(im1, im2, max_val=255, filter_size=11,
                          filter_sigma=1.5, k1=0.01, k2=0.03)

session = tf.compat.v1.Session()

result = session.run(ssim)

print(result)