SECTIONS
{
  .topic_gps :
  {
    __gps_subscriptions_start = .;
    *(.gps_subscription);
    __gps_subscriptions_end = .;
  } >RAM
}
