import cv2
import numpy as np
import projection
import argparse

def get_corners(vid):
    corners = []
    i = 0
    while True:
        ret, mask = vid.read()
        if(not ret):
            break
        i += 1
        try:
            corner = projection.get_corners_mask(mask)
            corners.append(corner)
        except Exception as e:
            print("Exception occurred in frame %d: " % (i - 1), e)
            break
    np_corners = np.array(corners)
    return np_corners

def save_corners(corners, file_path):
    # each line is of form: topl.x topl.y topr.x topr.y botr.x botr.y botl.x botl.y
    with open(file_path, "w") as fl:
        for cor in corners:
            for pt in cor:
                fl.write("%d %d " % (pt[0], pt[1]))
            fl.write("\n")

if __name__=="__main__":
    parser = argparse.ArgumentParser(
        description='Generate corner information from video mask')
    parser.add_argument('mask', metavar='M',
                        help='path to mask video')
    parser.add_argument('out', metavar='O',
                        help='path to the write the corner array')
    args = parser.parse_args()

    env_mask_path = args.mask
    outp_corner = args.out

    mask_vid = cv2.VideoCapture(env_mask_path)
    corners = get_corners(mask_vid)
    save_corners(corners,outp_corner)
