import cv2
import numpy as np
import keying
import projection
import argparse

def project_img(img, bg, mask, key_param):
    key_mask = keying.get_mask(img, key_param)
    bgra = keying.get_bgra(img, key_mask)
    res = projection.project_to_mask(bgra, mask)
    trans_img = res[:, :, :3]
    trans_mask = np.expand_dims((res[:, :, 3]).astype(np.float32) / 255, -1)
    result = (trans_mask * trans_img + (1 - trans_mask) * bg).astype(np.uint8)
    return result


def get_key_param(img):
    key_region = keying.get_region(img)
    return keying.get_params_ycrcb(img, key_region)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Realtime virtual studio')
    parser.add_argument('env', metavar='E',
                        help='path to environment video')
    parser.add_argument('mask', metavar='M',
                        help='path to mask video')
    parser.add_argument('vid', metavar='V',
                        help='path to the input green screen video')
    parser.add_argument('out', metavar='O',
                        help='path to the write the output video')
    parser.add_argument('--nowrite', action="store_true",
                        help='do not write the output video')
    parser.add_argument('--show',  action="store_true",
                        help='show the output video')
    parser.add_argument('--resize',  type=int,
                        help='resize the input by this factor')
    args = parser.parse_args()


    green_vid_path = args.vid
    env_vid_path = args.env
    env_mask_path = args.mask
    outp_vid = args.out

    cap_green = cv2.VideoCapture(green_vid_path)
    cap_vid = cv2.VideoCapture(env_vid_path)
    cap_mask = cv2.VideoCapture(env_mask_path)

    ret, img = cap_green.read()
    ret, bg = cap_vid.read()
    ret, mask = cap_mask.read()

    if args.resize != None:
        scale = int(args.resize)
        img = cv2.resize(img, (img.shape[1] // scale, img.shape[0] // scale))
        bg = cv2.resize(bg, (bg.shape[1] // scale, bg.shape[0] // scale))
        mask = cv2.resize(mask, (mask.shape[1] // scale, mask.shape[0] // scale))


    key_param = get_key_param(img)

    if not args.nowrite:
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        fps = cap_vid.get(5) ## get fps of video
        vid = cv2.VideoWriter(outp_vid, fourcc, fps, (bg.shape[1], bg.shape[0]))

    while True:
        ret1, img = cap_green.read()
        ret2, bg = cap_vid.read()
        ret3, mask = cap_mask.read()

        if (not(ret1 and ret2 and ret3)):
            break

        if args.resize!=None:
            scale = int(args.resize)
            img = cv2.resize(img,(img.shape[1]//scale,img.shape[0]//scale))
            bg = cv2.resize(bg,(bg.shape[1]//scale,bg.shape[0]//scale))
            mask = cv2.resize(mask,(mask.shape[1]//scale,mask.shape[0]//scale))

        res = project_img(img, bg, mask, key_param)

        if not args.nowrite:
            vid.write(res)
        
        if args.show:
            cv2.imshow("result",res)
            ret = cv2.waitKey(20)
            if ret==27:
                break
    cv2.destroyAllWindows()

    if not args.nowrite:
        vid.release()
