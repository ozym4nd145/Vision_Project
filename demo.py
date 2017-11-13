import cv2
import numpy as np
import keying
import projection
import argparse


def project_img(img, bg, mask_corners, key_param, tola=16, tolb=50, low_thresh=0.05, erode_sz=3,
                high_thresh=0.25, sz=5, space=200, sat_mul_lo=5, sat_mul_hi=7,scale_blur=20,blur_size=3):
    key_mask = keying.get_mask(
        img, key_param[0], tola, tolb, low_thresh, high_thresh, sz, space, erode_sz)
    mod_img = keying.process_img(img, key_param[1], sat_mul_lo, sat_mul_hi)

    bgra = keying.get_bgra(mod_img, key_mask)
    res = projection.project_to_mask(bgra, mask_corners, bg.shape)
    trans_img = res[:, :, :3]  # (x,y,3)
    trans_mask = np.expand_dims(
        (res[:, :, 3]).astype(np.float32) / 255, -1)  # (x,y)

#     r1 = np.multiply(trans_img,trans_mask)
    t_mask = np.tile(trans_mask, 3)

    trans_img = trans_img.astype(np.float32)
    bg = bg.astype(np.float32)

    r1 = cv2.multiply(trans_img, t_mask)
    r2 = cv2.multiply(bg, 1 - t_mask)

    result = cv2.add(r1, r2)
    result = result.astype(np.uint8)

    light_mask = np.expand_dims(
        scale_blur * cv2.blur(trans_mask * (1 - trans_mask), (blur_size, blur_size)), -1)
    # light_mask = keying.mod_mask(light_mask, 0, 1.0)
    light_mask = np.clip(light_mask, 0.0, 1.0)
    light = bg * light_mask
    diffl = result * (1 - light_mask)
    light_result = (light + diffl).astype(np.uint8)

    return light_result


def get_key_param(img):
    key_region = keying.get_region(img)
    ycrcb = keying.get_params_ycrcb(img, key_region)
    hls = keying.get_params_hls(img, key_region)
    return (ycrcb, hls)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Realtime virtual studio')
    parser.add_argument('env', metavar='E',
                        help='path to environment video')
    parser.add_argument('mask', metavar='M',
                        help='path to mask file')
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

    ret, img = cap_green.read()
    ret, bg = cap_vid.read()
    with open(env_mask_path, "r") as fl:
        lines = fl.read().split("\n")[:-1]
        corners = []
        for l in lines:
            p = [int(a) for a in l.split()]
            corners.append([[p[0], p[1]], [p[2], p[3]],
                            [p[4], p[5]], [p[6], p[7]]])
        corners = np.array(corners, dtype=np.float32)
        
    if args.resize != None:
        scale = int(args.resize)
        img = cv2.resize(img, (img.shape[1] // scale, img.shape[0] // scale))
        bg = cv2.resize(bg, (bg.shape[1] // scale, bg.shape[0] // scale))
        corners = corners/2


    key_param = get_key_param(img)

    if not args.nowrite:
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        fps = cap_vid.get(5) ## get fps of video
        vid = cv2.VideoWriter(outp_vid, fourcc, fps, (bg.shape[1], bg.shape[0]))

    iteration = 0
    end_iter = len(corners)
    while True:
        ret1, img = cap_green.read()
        ret2, bg = cap_vid.read()

        if (not(ret1 and ret2 and (iteration != end_iter))):
            break

        if args.resize!=None:
            scale = int(args.resize)
            img = cv2.resize(img,(img.shape[1]//scale,img.shape[0]//scale))
            bg = cv2.resize(bg,(bg.shape[1]//scale,bg.shape[0]//scale))

        res = project_img(img, bg, corners[iteration], key_param)

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
