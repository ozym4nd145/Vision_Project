import cv2
import numpy as np

def segment_ycrcb(orig, params, tola, tolb):
    ycrcb_im = cv2.cvtColor(orig, cv2.COLOR_BGR2YCrCb)
    Cb_key, Cr_key = params
    blue = ycrcb_im[:, :, 2]
    red = ycrcb_im[:, :, 1]

    diffbsq = (blue - Cb_key)**2
    diffrsq = (red - Cr_key)**2
    dist = np.sqrt(diffbsq + diffrsq).astype(np.float32)

    mask = ((dist - tola) / (tolb - tola)).astype(np.float32)
    mask[dist < tola] = 0.0
    mask[dist > tolb] = 1.0
    return mask


def get_region(img):
    r = cv2.selectROI("Select window of background", img=img)
    # r = cv2.selectROI("Select window of background", img=img,
    #                     fromCenter=False, showCrossair=False)
    cv2.destroyAllWindows()
    return r


def get_params_ycrcb(img, region):
    ycrcb_img = cv2.cvtColor(img, cv2.COLOR_BGR2YCrCb).astype(np.float32)
    cv2.destroyAllWindows()
    r = [int(x) for x in region]
    region = ycrcb_img[int(region[1]):int(
        region[1] + region[3]), int(region[0]):int(region[0] + region[2])]
    y_mean, Cr_mean, Cb_mean = np.mean(region, axis=(0, 1))
    y_std, Cr_std, Cb_std = np.std(region, axis=(0, 1))
    return [Cb_mean, Cr_mean]


def get_params_hls(img, region):
    hls_img = cv2.cvtColor(img, cv2.COLOR_BGR2HLS).astype(np.float32)
    r = [int(x) for x in region]
    region = hls_img[int(region[1]):int(region[1] + region[3]),
                     int(region[0]):int(region[0] + region[2])]
    h_mean, l_mean, s_mean = np.mean(region, axis=(0, 1))
    h_std, l_std, s_std = np.std(region, axis=(0, 1))
    return [h_mean, h_std, l_mean, l_std, s_mean, s_std]


def brighten(img, alpha, beta, gamma):
    cor_img = img.astype(np.float32)
    bright_img = alpha * cor_img + beta
    gam_cor = np.power(bright_img / 255, gamma) * 255
    bright_img = gam_cor.clip(0, 255).astype(np.uint8)
    return bright_img


def mod_mask(mask, low, high):
    mask = mask.copy()
    mask[mask > high] = 1.0
    mask[mask < low] = 0.0
    return mask


# def get_mask(img, bg, param_ycrcb, param_hls, tola=16, tolb=50, low_thresh=0.05, high_thresh=0.25, alpha=1.0, beta=0.0, gamma=1.0, sz=5, space=200, erode_sz=3):
def get_mask(img,param_ycrcb, tola=16, tolb=50, low_thresh=0.05, high_thresh=0.25, sz=5, space=200,erode_sz=2):
    ##ENSURE THAT param_ycrcb and param_hls correspond to brigthened img

    #     brimg = brighten(img,alpha,beta,gamma)
    brimg = img

    if not (sz<=0 or space<=1):
        brimg = cv2.bilateralFilter(brimg, sz, space, space)

    mask = segment_ycrcb(brimg, param_ycrcb, tola, tolb)
    mask = mod_mask(mask, low_thresh, high_thresh)

    if not(erode_sz <= 1):
        kernel = np.ones((erode_sz,erode_sz),np.uint8)
        mask = cv2.erode(mask,kernel,iterations = 1)
    return mask

def process_img(img,param_hls,mul0=20,mul1=30):
    hls_img = cv2.cvtColor(img, cv2.COLOR_BGR2HLS)
    h_mean = param_hls[0]
    h_std = param_hls[1]
    h_channel = hls_img[:, :, 0]
    
    low_th = mul0 * h_std
    hi_th = mul1 * h_std

    sat_mask = (np.abs(h_channel-h_mean)/(hi_th-low_th)) - (low_th/(hi_th-low_th))
    sat_mask = np.clip(sat_mask,0.0,1.0)

    hls_img[:, :, 2] = hls_img[:, :, 2] * sat_mask
    new_img = cv2.cvtColor(hls_img, cv2.COLOR_HLS2BGR).astype(np.uint8)

    return new_img

def get_bgra(img,mask):
    assert (len(mask.shape) == 2)  # (x,y)
    assert (len(img.shape) == 3)  # (x,y,3)
    alpha = (mask * 255).astype(np.uint8)
    alpha = np.expand_dims(alpha,-1) ##(x,y,1)
    return np.concatenate((img,alpha),axis=2) ##(x,y,4)
def write_alpha_img(img, mask, path):
    r_channel, g_channel, b_channel = cv2.split(img)
    alpha = (mask * 255).astype(np.uint8)
    img_RGBA = cv2.merge((r_channel, g_channel, b_channel, alpha))
    cv2.imwrite(path, img_RGBA)
    return img_RGBA
