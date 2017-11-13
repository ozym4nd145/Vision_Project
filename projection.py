import cv2
import numpy as np


def order_points(pts):
    # initialzie a list of coordinates that will be ordered
    # such that the first entry in the list is the top-left,
    # the second entry is the top-right, the third is the
    # bottom-right, and the fourth is the bottom-left
    rect = np.zeros((4, 2), dtype="float32")

    # the top-left point will have the smallest sum, whereas
    # the bottom-right point will have the largest sum
    s = pts.sum(axis=1)
    rect[0] = pts[np.argmin(s)]
    rect[2] = pts[np.argmax(s)]

    # now, compute the difference between the points, the
    # top-right point will have the smallest difference,
    # whereas the bottom-left will have the largest difference
    diff = np.diff(pts, axis=1)
    rect[1] = pts[np.argmin(diff)]
    rect[3] = pts[np.argmax(diff)]

    # return the ordered coordinates
    return rect


def get_corners_mask(mask):
    if len(mask.shape) > 2:
        mask = cv2.cvtColor(mask, cv2.COLOR_BGR2GRAY)
    corners = cv2.goodFeaturesToTrack(mask, 4, 0.01, 10)
    corners = np.int0(corners)
    corners = np.squeeze(corners)
    ## returns in order of (topl,bottoml,topr,bottomr)
    return order_points(corners)

def get_corners_img(img):
    ## in order of (topl,topr,bottomr,bottoml) in image coordinates
    return np.array([[0, 0], [(img.shape[1] - 1), 0],
                     [(img.shape[1] - 1), (img.shape[0] - 1)], [0, (img.shape[0] - 1)]], dtype=np.float32)

def get_transform(imgcorners,maskcorners):
    return cv2.getPerspectiveTransform(imgcorners, maskcorners)

def project_to_mask(img,mask_corner,shape):
    ## shape is the shape of mask in numpy dimension i.e. (y,x) in image coordinates
    img_corner = get_corners_img(img)
    transform = get_transform(img_corner,mask_corner)
    new_img = cv2.warpPerspective(
        img, transform, (shape[1], shape[0]))
    return new_img