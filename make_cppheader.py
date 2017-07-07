
import svdparser

# ------------------------------------------------------------------------------

SVD_FILE_NAME = "test.svd"

name_index = 0
description_index = 1

# ------------------------------------------------------------------------------

def write_peripheral(peripheral):
    for peripheral in peripheral:
        print type(peripheral)

# ------------------------------------------------------------------------------

def main():
    device_as_dict = svdparser.run(SVD_FILE_NAME)
    #print peripherals_as_dict

    device_name = device_as_dict.items()[0][0].lower()

    print device_name

    peripherals = device_as_dict.items()[0][1]

    for peripheral in peripherals.items():
        write_peripheral(peripheral[1:])
# ------------------------------------------------------------------------------

if __name__ == "__main__":
    main()
