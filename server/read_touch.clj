(use 'overtone.live)
(use 'overtone.inst.sampled-piano)
(require '[clojure.java.io :as io])
(import '[java.net ServerSocket])

(defn chr2int [c]       
  (-> (char c)
      (str)
      (Integer.)))

(defn beep [index]
  (let [my-scale (if (> index 6) (scale :C3 :major) (scale :C4 :major))
        note     (nth my-scale (if (> index 6) (- index 1) index))]
    (sampled-piano :note note)))


(defn receive
  [socket]
  (.readLine (io/reader socket)))

(defn serve [port handler]
  (with-open [server-sock (ServerSocket. port)
              sock (.accept server-sock)]
    (let [msg-in (receive sock)]
      (handler (chr2int (nth (seq msg-in) 0))))))


(while (pos? 1) (do (serve 12345 #(beep %))))