import pywavpack.pywavpack_ext as E
print(E.GetLibraryVersion())
print(E.GetLibraryVersionString())

array, rate = E.read("test.wv")
print(array.shape)

