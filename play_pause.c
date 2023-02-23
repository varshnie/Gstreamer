#include <stdio.h>
#include <string.h>
#include <gst/gst.h>
#include <pthread.h>



GstElement *pipeline = NULL;
GstBus *bus = NULL;
GstMessage *msg = NULL;
GstEvent *seek_event = NULL;

pthread_t bus_thread;
pthread_t option_thread;

void *bus_loop(void *arg);
void filepath();
void play();
void pause();
void stop();

void *bus_loop(void *arg)
{
    bus = gst_element_get_bus(pipeline);

    while (1) {
        msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

        if (msg == NULL) {
            continue;
        }

        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_EOS:
                g_print("End of stream message received\n");

                // Seek to the start of the video
                seek_event = gst_event_new_seek(1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, 0);
                gst_element_send_event(pipeline, seek_event);

                // Restart the pipeline
                gst_element_set_state(pipeline, GST_STATE_PLAYING);
                break;
            default:
                break;
        }

        gst_message_unref(msg);
    }

    gst_object_unref(bus);
}

void filepath()
{
    char path[100];
    char desc[1000];

    if (pipeline != NULL) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }

    printf("FILEPATH: ");
    scanf("%s", path);
    sprintf(desc, "playbin uri=file://%s", path);
    pipeline = gst_parse_launch(desc, NULL);
    pthread_create(&bus_thread, NULL, bus_loop, NULL);
}

void play()
{
    if (pipeline != NULL) {
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
    }
}

void pause()
{
    if (pipeline != NULL) {
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
    }
}

void stop()
{
    if (pipeline != NULL) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = NULL;
    }
}




void *option(void *arg)
{
  int input;
  
  

  printf("1.filepath\n2. play\n3. pause\n4. stop\n5. exit\n");

  while (1) {
    scanf("%d", &input);
    switch (input) {
      case 1:
        filepath();
        
        break;
      case 2:
        play();
        break;
      case 3:
        pause();
        break;
      case 4:
        stop();
        pthread_join(option_thread, NULL);
        printf("hi");
        break;
      case 5:
        stop();
        
        return NULL;
      default:
        break;
    }
  }
}

int main(int argc, char *argv[])
{
  gst_init(&argc, &argv);

  
 
  pthread_create(&option_thread, NULL, option, NULL);



  pthread_join(option_thread, NULL);

  return 0;
}
