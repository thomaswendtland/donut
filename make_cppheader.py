
import svdparser

# ------------------------------------------------------------------------------

SVD_FILE_NAME = "test.svd"

# ------------------------------------------------------------------------------

def write_register(register):
    print "writing register"

# ------------------------------------------------------------------------------

def main():
    peripherals_as_dict = svdparser.run(SVD_FILE_NAME)
    #print peripherals_as_dict

    for peripheral in peripherals_as_dict.items():
        print peripheral[1]
        print "\n"

# ------------------------------------------------------------------------------

if __name__ == "__main__":
    main()
