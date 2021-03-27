# JPEG-Compression
### JPEG Compression diagram
<img src="https://iie.fing.edu.uy/investigacion/grupos/gti/timag/trabajos/2015/embebido/compressed.fld/image002.png" alt="JPEG block diagram" width="50%">
1. RGB to YCbCr conversion, Here is the material for better understanding<br>

* https://sistenix.com/rgb2ycbcr.html<br>

2. Discrete Cosine Transform<br>

> The DCT is in a class of mathematical operations that includes the well known Fast Fourier Transform (FFT), as well as many others. The basic purpose of these operations is to take a signal and transform it from one type of representation to another. For example, an image is a two-dimensional signal that is perceived by the human visual system. The DCT can be used to convert the signal (spatial information) into numeric data ("frequency" or "spectral" information) so that the image’s information exists in a quantitative form that can be manipulated for compression.<br>

3. Quantization & zigzag<br>
<img src="https://static.javatpoint.com/tutorial/dip/images/jpeg-compression5.png">
> The zigzag scan is used to map the 8x8 matrix to a 1x64 vector. Zigzag scanning is used to group low-frequency coefficients to the top level of the vector and the high coefficient to the bottom. To remove the large number of zero in the quantized matrix, the zigzag matrix is used.

4. DPCM & RLE<br>
<img src="https://static.javatpoint.com/tutorial/dip/images/jpeg-compression7.png">
> Next step is vectoring, the different pulse code modulation (DPCM) is applied to the DC component. DC components are large and vary but they are usually close to the previous value. DPCM encodes the difference between the current block and the previous block.<br>
> Run Length Encoding (RLE) is applied to AC components. This is done because AC components have a lot of zeros in it. It encodes in pair of (skip, value) in which skip is non zero value and value is the actual coded value of the non zero components.

5. Huffman<br>
<img src="https://static.javatpoint.com/tutorial/dip/images/jpeg-compression9.png" alt="Huffman" >
> In this step, DC components are coded into Huffman.<br>

The remaining part of code is to revrse JPEG image back to bmp format, we can see the lossy image.
